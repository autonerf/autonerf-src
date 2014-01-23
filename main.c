// Standard C dependencies
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

// OpenCV dependencies
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc_c.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/filter.h>
#include <autonerf/vision.h>
#include <autonerf/launcher.h>
#include <debugger.h>

int allowed = 1;

void
timer_handler(int signum)
{
    allowed = 1;
}

void
set_timer(void)
{
    struct itimerval settings;
    settings.it_value.tv_sec     = 2;
    settings.it_value.tv_usec    = 0;
    settings.it_interval.tv_sec  = 0;
    settings.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &settings, NULL);
    signal(SIGALRM, timer_handler);
}

int
main(void)
{
    struct frame_t      frame;
    struct camera_t *   camera;
    IplImage *          grayscale = NULL;
    uint8_t             filtered[640][480];
    int32_t             position[2] = {-1, -1};
    int32_t             old_pos[2]  = {-1, -1};

    // Initialize and open the camera
    camera_init(&camera);
    camera_open(camera, 0);
    camera_set_filter(camera, filter_blueness);

    launcher_init();

    // Create windows: uncomment this if you want to use a normal PC
    // cvNamedWindow("input", 0);
    // cvNamedWindow("grayscale", 0);

    while (1) {
        if (!camera_frame_grab(camera, &frame)) {
            register uint8_t count = 0;

            contrast_stretch_fast((uint8_t *) frame.grayscale);
            threshold((uint8_t *) frame.grayscale, 100, 255);
            // threshold_iso_data((uint8_t *) frame.grayscale, BRIGHT);
            fill_holes(&frame, FOUR);
            memcpy(filtered, frame.grayscale, FRAME_SIZE);
            count = label_blobs(&frame, FOUR);
            blob_analyse(&frame, count, position);

            if (position[0] > 1 && position[1] > 1) {
                int center[] = { (FRAME_WIDTH / 2), (FRAME_HEIGHT / 2) };
                int offset[] = { center[0] - position[0], center[1] - position[1] };

                if ((offset[0] < 25 && offset[0] > -25) && (offset[1] < 25 && offset[1] > -25) && allowed &&
                    offset[0] != 1 && offset[1] != 1) {
                    allowed = 0;
                    launcher_shoot();
                    set_timer();
                } else {
                    printf("Offset: (%d, %d)\n", offset[0], offset[1]);

                    launcher_aim(offset[0], offset[1]);
                }

                old_pos[0] = position[0];
                old_pos[1] = position[1];
            } else {
                position[0] = old_pos[0];
                position[1] = old_pos[1];
            }



            // Uncomment this is you want to use a normal PC
            // contrast_stretch_fast((uint8_t *) filtered);
            // cvCircle(frame._frame, cvPoint(position[0], position[1]), 5, cvScalar(0, 255, 0, 0), 1, 8, 0);

            // grayscale = cvCreateImage(cvSize(FRAME_WIDTH, FRAME_HEIGHT), IPL_DEPTH_8U, 1);
            // grayscale->imageData = (char *) filtered;
            // cvShowImage("input", frame._frame);
            // cvShowImage("grayscale", grayscale);
            // cvReleaseImage(&grayscale);
            // debugger_save_image("image.ppm", frame.grayscale, FRAME_SIZE, TYPE_LABELED);
        }
        launcher_reload();
        cvWaitKey(100);
    }

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
