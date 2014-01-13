#ifndef AUTONERF_VISION_H
#define AUTONERF_VISION_H

#include <autonerf/camera.h>

/**
 Handels all vision operators.

 @param data the pointer to the image data
 @param pan  horisontal offset in pixels
 @param tilt vertical offset in pixels
 */
extern void vision_process(uint8_t* data, uint16_t* pan, uint16_t* tilt);

/**
 Enhances the contrast of a image.

 @param src the source data pointer
 @param dst the destination data pointer
 */
extern void contrast_stretch_fast(uint8_t* src, uint8_t* dst);

#endif /* AUTONERF_VISION_H */
