// Standard C dependencies
#include <stdio.h>

// OpenCV dependencies
#include <cv.h>
#include <highgui.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/filter.h>
#include <debugger.h>

int
main(void)
{
    struct frame_t      frame;
    struct camera_t *   camera;
    IplImage *          grayscale = NULL;

    // Initialize and open the camera
    camera_init(&camera);
    camera_open(camera, 0);
    camera_set_filter(camera, filter_blueness);

    // Create windows
    cvNamedWindow("input", 0);
    cvNamedWindow("grayscale", 0);

    while (1) {
        if (!camera_frame_grab(camera, &frame)) {
            grayscale = cvCreateImage(cvSize(FRAME_WIDTH, FRAME_HEIGHT), IPL_DEPTH_8U, 1);
            cvSetData(grayscale, frame.grayscale, FRAME_WIDTH);
            grayscale->align = 4;
            cvShowImage("input", frame._frame);
            cvShowImage("grayscale", grayscale);
            // debugger_save_image("image.ppm", (uint8_t *) frame.grayscale, FRAME_SIZE, TYPE_DEFAULT);
        }
    }

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
