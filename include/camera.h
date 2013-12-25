#ifndef AUTONERF_CAMERA_H
#define AUTONERF_CAMERA_H

// Standard C dependencies
#include <errno.h>
#include <stdint.h>
#include <string.h>

// POSIX dependencies
#include <sys/ioctl.h>

// External dependencies
#include <libv4l2.h>
#include <linux/videodev2.h>

struct buffer_t {
    void *  data;
    size_t  size;
};

struct pixel_t {
    uint8_t red;
    uint8_t blue;
    uint8_t green;
};

struct frame_t {
    size_t              size;
    struct pixel_t *    pixels;
    struct v4l2_buffer  buffer;
};

struct camera_t {
    int                 fd;
    struct buffer_t *   buffers;
    size_t              buffer_count;
};

extern inline int xioctl(int fd, int request, void * arguments)
{
    int result;

    do {
        result = ioctl(fd, request, arguments);
    } while (result == -1 && (errno == EINTR || errno == EAGAIN));

    if (result == -1) {
        fprintf(stderr, "%s [%d]\n", strerror(errno), errno);

        return -1;
    }

    return 0;
}

/**
 * Initialize a camera structure
 *
 * @param camera The camera structure to initialize
 */
extern int camera_init(struct camera_t ** camera);

/**
 * Deinitialize a camera structure
 * @param camera The camera structure to deinitialize
 */
extern int camera_deinit(struct camera_t ** camera);

/**
 * Open a camera
 *
 * @param camera A pointer to the camera structure to use
 * @param device The camera device to open
 */
extern int camera_open(struct camera_t * camera, const char * device);

/**
 * Close a camera
 *
 * @param camera A pointer to the camera structure to use
 */
extern int camera_close(struct camera_t * camera);

/**
 * Grab a camera frame
 *
 * @param  camera The camera to use
 * @param  frame  The frame to grab
 */
extern int camera_frame_grab(struct camera_t * camera, struct frame_t * frame);

/**
 * Start grabbing frames using a camera
 *
 * @param  camera The camera to use
 */
extern inline int camera_start(struct camera_t * camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return xioctl(camera->fd, VIDIOC_STREAMON, &type);
}

/**
 * Stop grabbing frames using a camera
 *
 * @param  camera The camera to use
 */
extern inline int camera_stop(struct camera_t * camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return xioctl(camera->fd, VIDIOC_STREAMOFF, &type);
}

/**
 * Release a camera frame
 *
 * @param  camera The camera to use
 * @param  frame  The frame to release
 */
extern inline int camera_frame_release(struct camera_t * camera, struct frame_t * frame)
{
    if (xioctl(camera->fd, VIDIOC_QBUF, &frame->buffer) < 0) {
        return -1;
    }

    return 0;
}

#endif /* AUTONERF_CAMERA_H */
