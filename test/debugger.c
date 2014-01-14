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

    frame.pixels = (struct pixel_t *) calloc(size, sizeof(struct pixel_t));

    for (i = 0; i < size; i++) {
        if ((image[i]) == 0) {
            frame.pixels[i].red   = 0;
            frame.pixels[i].green = 0;
            frame.pixels[i].blue  = 0;
        } else if ((image[i] % 3) == 0) {
            frame.pixels[i].red   = 0;
            frame.pixels[i].green = 0;
            frame.pixels[i].blue  = 255;
        } else if ((image[i] % 2) == 0) {
            frame.pixels[i].red   = 0;
            frame.pixels[i].green = 255;
            frame.pixels[i].blue  = 0;
        } else {
            frame.pixels[i].red   = 255;
            frame.pixels[i].green = 0;
            frame.pixels[i].blue  = 0;
        }
    }

    FILE * output = fopen(filename, "w");
    fprintf(output, "P6\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);
    fwrite(frame.pixels, sizeof(struct pixel_t), size, output);
    fclose(output);

    free(frame.pixels);
}

void
debugger_save_image(const char * filename, const uint8_t * image, const size_t size, const uint8_t type)
{
    if (type == TYPE_LABELED) {
        debugger_save_labeled_image(filename, image, size);
    } else {
        FILE * output = fopen(filename, "w");
        fprintf(output, "P5\n%d %d\n255\n", FRAME_WIDTH, FRAME_HEIGHT);
        fwrite(image, sizeof(uint8_t), size, output);
        fclose(output);
    }
}
