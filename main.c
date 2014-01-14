// Standard C++ dependencies
#include <time.h>
#include <stdio.h>
#include <stdint.h>

// Project dependencies
#include <autonerf/camera.h>
#include <autonerf/logger.h>
#include <autonerf/filter.h>

int
main(void)
{
    struct frame_t      frame;
    struct camera_t *   camera;
    struct timespec     benchmarks[3];
    double              delta  = 0.0;
    FILE *              output = fopen("grayscale.ppm", "w");

    // Initialize and open the camera
    camera_init(&camera);

    if (camera_open(camera, "/dev/video0")) {
        LOG_CRITICAL("Could not open camera: %s", "/dev/video0");
        camera_deinit(&camera);

        return -1;
    }

    camera_set_filter(camera, filter_redness);

    camera_start(camera);

    // Start benchmarking
    clock_gettime(CLOCK_REALTIME, &benchmarks[0]);
    camera_frame_grab(camera, &frame);
    clock_gettime(CLOCK_REALTIME, &benchmarks[1]);

    fprintf(output, "P5\n640 480 255\n");
    fwrite(frame.filtered, sizeof(uint8_t), frame.size, output);
    fclose(output);

    camera_stop(camera);

    delta = (benchmarks[1].tv_sec - benchmarks[0].tv_sec);
    delta = delta + (((double) (benchmarks[1].tv_nsec - benchmarks[0].tv_nsec)) / 1000000000.0d);

    printf("Duration: %fs\n", delta);

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
