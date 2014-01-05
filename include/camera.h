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
int camera_start(struct camera_t * camera);

/**
 * Stop grabbing frames using a camera
 *
 * @param  camera The camera to use
 */
extern int camera_stop(struct camera_t * camera);

/**
 * Release a camera frame
 *
 * @param  camera The camera to use
 * @param  frame  The frame to release
 */
int camera_frame_release(struct camera_t * camera, struct frame_t * frame);

#endif /* AUTONERF_CAMERA_H */
