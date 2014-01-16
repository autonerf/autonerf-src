#ifndef AUTONERF_VISION_H
#define AUTONERF_VISION_H

#include <autonerf/camera.h>

enum eBrightness
{
    BRIGHT = 0,
    DARK = 1
};

enum eConnected{
    EIGHT = 8,
    FOUR = 4
};

/**
 * List of improvements:
 * - No copy data, src pointer == dst pointer?
 * - Smaller check neighbour function?
 * Possible bottelnecks:
 * - Set selected to value
 * - Check neighbour function
 */

/**
 Handels all vision operators.

 @param data the pointer to the image data
 @param pan  horisontal offset in pixels
 @param tilt vertical offset in pixels
 */
extern void vision_process(uint8_t* img, uint16_t * pan, uint16_t * tilt);

/**
 Enhances the contrast of a image.

 @param img the image
 */
extern void contrast_stretch_fast(uint8_t* img);

/**
 Automatic threshold to a grayscale image

 @param img the image
 @param brightness negative/positive image
 */
extern void threshold_iso_data(uint8_t* img, enum eBrightness brightness);

/**
 Fills all blob holes.

 @param img the image
 @param connected four or eight connected pixels
 */
extern void fill_holes(uint8_t* img, enum eConnected connected);

/**
 Removes the border blobs.

 @param img the image
 @param connected four or eight connected pixels
 */
extern void remove_border_blobs(uint8_t* img, enum eConnected connected);

/**
 Labels all blobs in a image

 @param img the image
 @param connected four or eight connected pixels
 */
extern uint32_t label_blobs(uint8_t* img, enum eConnected connected);

/**
 Analyses the image and returns the coordinates of the largest blob.

 @param img the image
 @param blobcount the number of blobs
 @param blob_pos_x the x center position of the blob
 @param blob_pos_y the y center position of the blob
 */
extern void blob_analyse(uint8_t *img, uint8_t blobcount, uint32_t* blob_pos_x, uint32_t* blob_pos_y);

#endif /* AUTONERF_VISION_H */
