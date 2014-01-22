// Standard C dependencies
#include <stdio.h>

// OpenCV dependencies
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc_c.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/filter.h>
#include <autonerf/vision.h>
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
            contrast_stretch_fast((uint8_t *) frame.grayscale);
            threshold_iso_data((uint8_t *) frame.grayscale, BRIGHT);
            fill_holes(&frame, FOUR);
            printf("Found %d blobs\n", label_blobs(&frame, EIGHT));

            grayscale = cvCreateImage(cvSize(FRAME_WIDTH, FRAME_HEIGHT), IPL_DEPTH_8U, 1);
            grayscale->imageData = (char *) frame.grayscale;
            cvShowImage("input", frame._frame);
            cvShowImage("grayscale", grayscale);
            cvReleaseImage(&grayscale);
            debugger_save_image("image.ppm", frame.grayscale, FRAME_SIZE, TYPE_LABELED);
        }
    }

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
