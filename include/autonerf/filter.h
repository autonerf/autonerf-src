#ifndef AUTONERF_FILTER_H
#define AUTONERF_FILTER_H

// Standard C dependencies
#include <stdint.h>

// Project dependencies
#include <autonerf/camera.h>

extern void filter_blackness(const struct frame_t * __restrict frame, uint8_t * destination);
extern void filter_redness(const struct frame_t * __restrict frame, uint8_t * destination);
extern void filter_blueness(const struct frame_t * __restrict frame, uint8_t * destination);
extern void filter_greeness(const struct frame_t * __restrict frame, uint8_t * destination);

#endif /* AUTONERF_FILTER_H */
