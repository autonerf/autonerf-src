// Standard C++ dependencies
#include <time.h>
#include <stdio.h>
#include <stdint.h>

// Project dependencies
#include <camera.h>

#ifndef FRAME_COUNT
    #define FRAME_COUNT 30
#endif

int
main(void)
{
    int                 i;
    struct frame_t      frame;
    struct camera_t *   camera;
    struct timespec     benchmarks[3];
    double              fps   = 0.0;
    double              delta = 0.0;

    // Initialize and open the camera
    camera_init(&camera);
    camera_open(camera, "/dev/video0");

    // Start benchmarking
    camera_start(camera);
    clock_gettime(CLOCK_REALTIME, &benchmarks[0]);
    for (i = 0; i < FRAME_COUNT; i++) {
        camera_frame_grab(camera, &frame);
        camera_frame_release(camera, &frame);
    }
    clock_gettime(CLOCK_REALTIME, &benchmarks[1]);
    camera_stop(camera);

    delta = (benchmarks[1].tv_sec - benchmarks[0].tv_sec);
    delta = delta + (((double) (benchmarks[1].tv_nsec - benchmarks[0].tv_nsec)) / 1000000000.0d);
    fps   = FRAME_COUNT / delta;

    printf("Duration: %fs\n", delta);
    printf("FPS: %f (%d / %f)\n", fps, FRAME_COUNT, delta);

    // Close and deinitialize the camera
    camera_close(camera);
    camera_deinit(&camera);

    return 0;
}
