// Project dependencies
#include <autonerf/filter.h>

void
filter_redness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (i = 0; i < FRAME_SIZE; i++) {
        int value   = frame->pixels[i].red - (frame->pixels[i].green + frame->pixels[i].blue);
            dest[i] = (value < 0) ? 0 : value;
    }
}

void
filter_blueness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (; i > 0; i--) {
        int16_t value   = frame->pixels[(i - 1)].blue - (frame->pixels[(i - 1)].green + frame->pixels[(i - 1)].red);
                dest[i] = value;
    }
}

void
filter_greeness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (; i > 0; i--) {
        int16_t value   = frame->pixels[(i - 1)].green - (frame->pixels[(i - 1)].red + frame->pixels[(i - 1)].blue);
                dest[i] = value;
    }
}
