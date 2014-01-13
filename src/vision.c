#include <autonerf/vision.h>


void
vision_process(uint8_t* data, uint16_t* pan, uint16_t* tilt)
{
    contrast_stretch_fast(data, data);
    threshold_iso_data(data, data, DARK);
    fill_holes(data, data);
    remove_border_blobs(data, data);
    label_blobs(data, data);
    blob_analyse(data, info);

    //Search largest blob

}

void 
contrast_stretch_fast(uint8_t* src, uint8_t* dst)
{
    register uint32_t i;
    register uint8_t lPix = 255;
    register uint8_t hPix = 0;
    register float verhouding = 0.0f;
    register float lutVal;
    uint8_t lut[256];
    register uint8_t* srcPt = src;
    register uint8_t* dstPt = dst;

    //Determin lowist and highest value
    for(i = FRAME_SIZE; i > 0; i--){
        if(*srcPt > hPix){
            hPix = *srcPt;
        }
        if(*srcPt < lPix){
            lPix = *srcPt;
        }
        srcPt++;
    }

    if(lPix != hPix){
        verhouding = (255.0f) / ((float)(hPix - lPix));
    } else {
        dst = src;
        return;
    }

    //Create LUT
    for(i = 255; i > 0; i--){
        lutVal = (((float)(i)) * verhouding) + 0.5f;

        //Overflow protection
        if(lutVal > 255.0f){
            lutVal = 255.0f;
        }
        if(lutVal < 0.0f){
            lutVal = 0.0;
        }

        lut[i] = (uint8_t)(lutVal);
    }
    lut[0] = 0;

    srcPt = src;
    dstPt = dst;

    //Create output file
    for(i = FRAME_SIZE; i > 0; i--){
        *dstPt = lut[*srcPt];
        srcPt++;
        dstPt++;
    }
}

void
thresholdIsoData(image_t *src, image_t *dst, brightness_t brightness)
{
  uint16_t histogram[HISTOGRAMSIZE];
  uint32_t som;
  register uint32_t imageSize = src->width * src->height;
  register uint32_t average = 0;
  register int32_t  i;

  /* Set histogram to 0 */
  for(i = (HISTOGRAMSIZE - 1); i > 0; i--){
      histogram[i] = 0;
  }

  vHistogram(src, &histogram[0], &som);

  /* Determin start point */
  for(i = 0; histogram[i] > 0; i++){
    i = i;
  }

  /* Calculate average */
  average = i + (som / imageSize);
  if(brightness == DARK){
    vThreshold(src, dst, 0, average);
  } else {
    vThreshold(src, dst, average, 255);
  }
}
