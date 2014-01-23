// Standard C dependencies
#include <stdio.h>

// OpenCV dependencies
#include <cv.h>
#include <highgui.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/logger.h>

int
camera_init(struct camera_t ** camera)
{
    (*camera)         = (struct camera_t *) malloc(sizeof(struct camera_t));
    (*camera)->device = NULL;
    (*camera)->filter = NULL;

    return 0;
}

int
camera_set_filter(struct camera_t * camera, camera_filter_t filter)
{
    camera->filter = filter;

    return 0;
}

int
camera_unset_filter(struct camera_t * camera)
{
    camera->filter = NULL;

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
    if ((*camera)->device) {
        camera_close((*camera));
    }

    // Free camera memory
    free((*camera));
    (*camera) = NULL;

    return 0;
}

int
camera_open(struct camera_t * camera, const int device)
{
    if (camera->device) {
        return -1;
    }

    camera->device = cvCaptureFromCAM(device);
    cvSetCaptureProperty(camera->device, CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cvSetCaptureProperty(camera->device, CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    // cvSetCaptureProperty(camera->device, CV_CAP_PROP_CONTRAST, 0);
    // cvSetCaptureProperty(camera->device, CV_CAP_PROP_SATURATION, 127);

    return camera->device ? 0 : -1;
}

int
camera_close(struct camera_t * camera)
{
    if (!camera->device) {
        return -1;
    }

    cvReleaseCapture(&camera->device);
    camera->device = NULL;

    return 0;
}

int
camera_frame_grab(struct camera_t * camera, struct frame_t * frame)
{
    if (!camera->device) {
        return -1;
    }

    frame->_frame = cvQueryFrame(camera->device);
    frame->pixels = (struct pixel_t *) frame->_frame->imageData;

    if (!frame->_frame || !frame->_frame->imageSize) {
        return -1;
    }

    if (camera->filter) {
        camera->filter(frame, (uint8_t *) frame->grayscale);
    }

    return 0;
}

int
camera_frame_release(struct camera_t * camera, struct frame_t * frame)
{
    return 0;
}

int
camera_start(struct camera_t * camera)
{
    return 0;
}

int
camera_stop(struct camera_t * camera)
{
    return 0;
}
