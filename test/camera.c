// Standard C dependencies
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/camera.h>

#ifndef CAMERA_FRAME_IMAGE
#define CAMERA_FRAME_IMAGE "resources/test/camera.frame"
#endif

int
camera_init(struct camera_t ** camera)
{
    (*camera)                   = (struct camera_t *) malloc(sizeof(struct camera_t));
    (*camera)->fd               = -1;
    (*camera)->filter           = NULL;
    (*camera)->buffers          = NULL;
    (*camera)->buffer_count     = 0;

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
    if (!(*camera)) {
        return -1;
    }

    if ((*camera)->fd != -1) {
        camera_close((*camera));
    }

    free((*camera));
    (*camera) = NULL;

    return 0;
}

int
camera_open(struct camera_t * camera, const char * device)
{
    device     = 0;
    camera->fd = 0;

    return 0;
}

int
camera_close(struct camera_t * camera)
{
    camera->fd = -1;

    return 0;
}

int
camera_frame_grab(struct camera_t * camera, struct frame_t * frame)
{
    FILE * frame_file = fopen(CAMERA_FRAME_IMAGE, "r");

    if (!frame_file) {
        LOG_ERROR("%s", strerror(errno));

        return -1;
    }

    frame->pixels = (struct pixel_t *) calloc(FRAME_SIZE, sizeof(struct pixel_t));
    frame->size   = fread(frame->pixels, sizeof(struct pixel_t), FRAME_SIZE, frame_file);

    if (camera->filter) {
        frame->filtered = (uint8_t *) calloc(FRAME_SIZE, sizeof(uint8_t));
        camera->filter(frame, frame->filtered);
    } else {
        frame->filtered = NULL;
    }

    return 0;
}

int
camera_frame_release(struct camera_t * camera, struct frame_t * frame)
{
    camera = 0;

    if (frame->filtered) {
        free(frame->filtered);
        frame->filtered = NULL;
    }

    free(frame->pixels);

    return 0;
}

int
camera_start(struct camera_t * camera)
{
    camera = 0;

    return 0;
}

int
camera_stop(struct camera_t * camera)
{
    camera = 0;

    return 0;
}
