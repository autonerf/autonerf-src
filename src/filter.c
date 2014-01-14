// Project dependencies
#include <autonerf/filter.h>

void
filter_redness(struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (; i > 0; i--) {
        int16_t value   = frame->pixels[(i - 1)].red - (frame->pixels[(i - 1)].green + frame->pixels[(i - 1)].blue);
                dest[(i - 1)] = value;
    }
}

void
filter_blueness(struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (; i > 0; i--) {
        int16_t value   = frame->pixels[(i - 1)].blue - (frame->pixels[(i - 1)].green + frame->pixels[(i - 1)].red);
                dest[i] = value;
    }
}

void
filter_greeness(struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t i = FRAME_SIZE;

    for (; i > 0; i--) {
        int16_t value   = frame->pixels[(i - 1)].green - (frame->pixels[(i - 1)].red + frame->pixels[(i - 1)].blue);
                dest[i] = value;
    }
}
