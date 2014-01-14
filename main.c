// Standard C++ dependencies
#include <time.h>
#include <stdio.h>
#include <stdint.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/logger.h>
#include <autonerf/filter.h>
#include <autonerf/vision.h>

// Testing dependencies
#include <debugger.h>

int
main(void)
{
    struct frame_t      frame;
    struct camera_t *   camera;

    // Initialize and open the camera
    camera_init(&camera);

    if (camera_open(camera, "/dev/video0")) {
        LOG_CRITICAL("Could not open camera: %s", "/dev/video0");
        camera_deinit(&camera);

        return -1;
    }

    camera_set_filter(camera, filter_redness);
    camera_start(camera);
    camera_frame_grab(camera, &frame);
    camera_stop(camera);

    // Write frame to output
    threshold_iso_data(frame.filtered, BRIGHT);
    label_blobs(frame.filtered, EIGHT);
    debugger_save_image("grayscale.ppm", frame.filtered, FRAME_SIZE, TYPE_LABELED);

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
