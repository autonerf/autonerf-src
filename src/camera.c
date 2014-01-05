// Standard C dependencies
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// POSIX dependencies
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

// Project dependencies
#include <camera.h>
#include <logger.h>

inline int xioctl(int fd, int request, void * arguments)
{
    int result;

    do {
        result = v4l2_ioctl(fd, request, arguments);
    } while (result == -1 && (errno == EINTR || errno == EAGAIN));

    if (result == -1) {
        LOG_ERROR(strerror(errno));
    }

    return result;
}

int
camera_init(struct camera_t ** camera)
{
    (*camera)                   = (struct camera_t *) malloc(sizeof(struct camera_t));
    (*camera)->fd               = -1;
    (*camera)->buffers          = NULL;
    (*camera)->buffer_count     = 0;

    return 0;
}

int
camera_deinit(struct camera_t ** camera)
{
    // Check if memory is initialized
    if (!(*camera)) {
        return -1;
    }

    // Check if the camera is openend
    if ((*camera)->fd != -1) {
        camera_close((*camera));
    }

    // Free camera memory
    free((*camera));
    (*camera) = NULL;

    return 0;
}

int
camera_open(struct camera_t * camera, const char * device)
{
    size_t                      i;
    struct v4l2_format          format;
    struct v4l2_buffer          buffer;
    struct v4l2_requestbuffers  request;

    if (!camera || camera->fd != -1) {
        return -1;
    }

    // Try to open the camera device
    if ((camera->fd = v4l2_open(device, O_RDWR | O_NONBLOCK)) < 0) {
        LOG_ERROR(strerror(errno));

        return -1;
    }

    // Try to set the camera format
    memset(&format, 0, sizeof(struct v4l2_format));
    format.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width        = FRAME_WIDTH;
    format.fmt.pix.height       = FRAME_HEIGHT;
    format.fmt.pix.pixelformat  = V4L2_PIX_FMT_RGB24;
    format.fmt.pix.field        = V4L2_FIELD_INTERLACED;

    if (xioctl(camera->fd, VIDIOC_S_FMT, &format) != 0) {
        return -1;
    }

    if (format.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) {
        LOG_CRITICAL("libv4l2 did not accept format RGB24, cannot continue");
        camera_close(camera);

        return -1;
    }

    // Try to request buffers for the camera
    memset(&request, 0, sizeof(struct v4l2_requestbuffers));
    request.count = 2;
    request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    request.memory = V4L2_MEMORY_MMAP;

    if (xioctl(camera->fd, VIDIOC_REQBUFS, &request) < 0) {
        return -1;
    }

    camera->buffers = (struct buffer_t *) calloc(request.count, sizeof(struct buffer_t));

    // Memory map buffers to driver
    for (camera->buffer_count = 0; camera->buffer_count < request.count; camera->buffer_count++) {
        memset(&buffer, 0, sizeof(struct v4l2_buffer));
        buffer.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory   = V4L2_MEMORY_MMAP;
        buffer.index    = camera->buffer_count;

        if (xioctl(camera->fd, VIDIOC_QUERYBUF, &buffer) < 0) {
            return -1;
        }

        camera->buffers[camera->buffer_count].size = buffer.length;
        camera->buffers[camera->buffer_count].data = v4l2_mmap(
            NULL,
            buffer.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            camera->fd,
            buffer.m.offset
        );

        if (camera->buffers[camera->buffer_count].data == MAP_FAILED) {
            LOG_ERROR(strerror(errno));

            return -1;
        }
    }

    for (i = 0; i < camera->buffer_count; i++) {
        memset(&buffer, 0, sizeof(struct v4l2_buffer));
        buffer.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory   = V4L2_MEMORY_MMAP;
        buffer.index    = i;

        if (xioctl(camera->fd, VIDIOC_QBUF, &buffer)) {
            return -1;
        }
    }

    return 0;
}

int
camera_close(struct camera_t * camera)
{
    size_t i;

    if (!camera) {
        return -1;
    }

    // Unmap the buffer memory
    for (i = 0; i < camera->buffer_count; i++) {
        v4l2_munmap(camera->buffers[i].data, camera->buffers[i].size);
    }

    // Close the camera device
    v4l2_close(camera->fd);
    free(camera->buffers);

    // Clean up
    camera->fd              = -1;
    camera->buffers         = NULL;
    camera->buffer_count    = 0;

    return 0;
}

int
camera_frame_grab(struct camera_t * camera, struct frame_t * frame)
{
    fd_set              read;
    int                 result;
    struct timeval      timeout;

    do {
        FD_ZERO(&read);
        FD_SET(camera->fd, &read);

        timeout.tv_sec  = 0;
        timeout.tv_usec = 5;

        result = select(camera->fd + 1, &read, NULL, NULL, &timeout);
    } while (result < 0 && (errno = EINTR));

    memset(&frame->buffer, 0, sizeof(struct v4l2_buffer));
    frame->buffer.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frame->buffer.memory = V4L2_MEMORY_MMAP;

    xioctl(camera->fd, VIDIOC_DQBUF, &frame->buffer);

    frame->size   = frame->buffer.bytesused;
    frame->pixels = (struct pixel_t *) camera->buffers[frame->buffer.index].data;

    return 0;
}

int
camera_frame_release(struct camera_t * camera, struct frame_t * frame)
{
    if (xioctl(camera->fd, VIDIOC_QBUF, &frame->buffer) < 0) {
        return -1;
    }

    return 0;
}

int
camera_start(struct camera_t * camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return xioctl(camera->fd, VIDIOC_STREAMON, &type);
}

int
camera_stop(struct camera_t * camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return xioctl(camera->fd, VIDIOC_STREAMOFF, &type);
}
