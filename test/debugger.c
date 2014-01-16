// Standard C dependencies
#include <stdio.h>
#include <stdlib.h>

// Project dependencies
#include <autonerf/camera.h>

// Testing dependencies
#include <debugger.h>

void
debugger_save_labeled_image(const char * filename, const uint8_t * image, const size_t size)
{
    size_t         i;
    struct frame_t frame;
    size_t         count;

    frame.pixels = (struct pixel_t *) calloc(size, sizeof(struct pixel_t));

    for (i = 0; i < size; i++) {
        if ((image[i]) == 0) {
            frame.pixels[i] = (struct pixel_t) {0, 0, 0};
        } else if ((image[i] % 3) == 0) {
            frame.pixels[i] = (struct pixel_t) {0, 0, 255};
        } else if ((image[i] % 2) == 0) {
            frame.pixels[i] = (struct pixel_t) {0, 255, 0};
        } else {
            frame.pixels[i] = (struct pixel_t) {255, 0, 0};
        }
    }

    FILE * output = fopen(filename, "w");
    fprintf(output, "P6\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);
    count = (size_t) fwrite(frame.pixels, sizeof(struct pixel_t), size, output);
    fclose(output);

    free(frame.pixels);

    printf("Wrote %lu bytes\n", count);
}

void
debugger_save_thresholded_image(const char * filename, const uint8_t * image, const size_t size)
{
    size_t  i;
    size_t  count  = 0;
    FILE *  output = fopen(filename, "w");
    uint8_t data[size];

    fprintf(output, "P5\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);
    for (i = 0; i < size; i++) {
        data[i] = (image[i] != 0) ? 0 : 255;
    }
    count = fwrite(data, sizeof(uint8_t), size, output);
    fclose(output);

    printf("Wrote %lu bytes\n", count);
}

void
debugger_save_image(const char * filename, const uint8_t * image, const size_t size, const uint8_t type)
{
    if (type == TYPE_LABELED) {
        debugger_save_labeled_image(filename, image, size);
    } else if (type == TYPE_THRESHOLDED) {
        debugger_save_thresholded_image(filename, image, size);
    } else {
        size_t count  = 0;
        FILE * output = fopen(filename, "w");
        fprintf(output, "P5\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);
        count = fwrite(image, sizeof(uint8_t), size, output);
        fclose(output);

        printf("Wrote %lu bytes\n", count);
    }
}
