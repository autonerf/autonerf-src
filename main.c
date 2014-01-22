// OpenCV dependencies
#include <cv.h>
#include <highgui.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/filter.h>

int
main(void)
{
    struct frame_t      frame;
    struct camera_t *   camera;
    IplImage *          grayscale = cvCreateImage(cvSize(FRAME_HEIGHT, FRAME_WIDTH), IPL_DEPTH_8U, 1);

    // Initialize and open the camera
    camera_init(&camera);
    camera_open(camera, 0);
    camera_set_filter(camera, filter_blueness);

    // Create windows
    cvNamedWindow("input", 0);
    cvNamedWindow("grayscale", 0);

    while (1) {
        camera_frame_grab(camera, &frame);

        cvSetData(grayscale, frame.grayscale, FRAME_WIDTH);
        cvShowImage("input", frame._frame);
        cvShowImage("grayscale", grayscale);
    }

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
