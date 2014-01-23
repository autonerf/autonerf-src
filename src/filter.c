// Project dependencies
#include <stdio.h>
#include <autonerf/filter.h>

void
filter_blackness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t    line_count       = frame->_frame->height;
    size_t    element_count    = frame->_frame->width * frame->_frame->nChannels;
    size_t    step             = frame->_frame->widthStep;
    uint8_t * data             = (uint8_t *) frame->_frame->imageData;
    size_t    k                = 0;

    for (size_t i = 1; i < line_count; i++) {
        for (size_t j = 0; j < element_count; j += frame->_frame->nChannels) {
            register int value = -1 * ~(data[j] + data[j + 1] + data[j + 2]);
            dest[k++] = value < 0 ? 0 : value;
        }

        data += step;
    }
}

void
filter_redness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t    line_count       = frame->_frame->height;
    size_t    element_count    = frame->_frame->width * frame->_frame->nChannels;
    size_t    step             = frame->_frame->widthStep;
    uint8_t * data             = (uint8_t *) frame->_frame->imageData;
    size_t    k                = 0;

    for (size_t i = 1; i < line_count; i++) {
        for (size_t j = 0; j < element_count; j += frame->_frame->nChannels) {
            register int value = data[j + 2] - (data[j] + data[j + 1]);
            dest[k++] = (value < 0) ? 0 : value;
        }

        data += step;
    }
}

void
filter_blueness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t    line_count       = frame->_frame->height;
    size_t    element_count    = frame->_frame->width * frame->_frame->nChannels;
    size_t    step             = frame->_frame->widthStep;
    uint8_t * data             = (uint8_t *) frame->_frame->imageData;
    size_t    k                = 0;

    for (size_t i = 0; i < line_count; i++) {
        for (size_t j = 0; j < element_count; j += frame->_frame->nChannels) {
            register int value = data[j] - (data[j + 1] + data[j + 2]);
            dest[k++] = (value < 0) ? 0 : value;
        }

        data += step;
    }
}

void
filter_greeness(const struct frame_t * __restrict frame, uint8_t * dest)
{
    size_t    line_count       = frame->_frame->height;
    size_t    element_count    = frame->_frame->width * frame->_frame->nChannels;
    size_t    step             = frame->_frame->widthStep;
    uint8_t * data             = (uint8_t *) frame->_frame->imageData;
    size_t    k                = 0;

    for (size_t i = 1; i < line_count; i++) {
        for (size_t j = 0; j < element_count; j += frame->_frame->nChannels) {
            register int value = data[j + 1] - (data[j] + data[j + 2]);
            dest[k++] = (value < 0) ? 0 : value;
        }

        data += step;
    }
}
