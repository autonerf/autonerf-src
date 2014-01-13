#include <autonerf/vision.h>

//Local prototypes
void threshold(uint8_t* src, uint8_t* dst, uint8_t low, uint8_t high);
void histogram(uint8_t* src, uint16_t *hist, uint32_t *sum);
uint8_t neighbour_count(uint8_t* src, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
uint8_t neighbours_equal_or_higher(uint8_t* src, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
void set_selected_to_value(uint8_t* src, uint8_t* dst, uint8_t selected, uint8_t value);

void
vision_process(uint8_t* data, uint16_t* pan, uint16_t* tilt)
{
    contrast_stretch_fast(data, data);
    threshold_iso_data(data, data, DARK);
    fill_holes(data, data, FOUR);
    remove_border_blobs(data, data, EIGHT);
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
threshold_iso_data(uint8_t* src, uint8_t* dst, enum eBrightness brightness)
{
  uint16_t histogram[256];
  uint32_t som;
  register uint8_t average = 0;
  register int32_t  i;

  /* Set histogram to 0 */
  for(i = 255; i >= 0; i--){
      histogram[i] = 0;
  }

  histogram(src, &histogram[0], &som);

  /* Determin start point */
  for(i = 0; histogram[i] > 0; i++){
    i = i;
  }

  /* Calculate average */
  average = (uint8_t)(i + (int32_t)(som / FRAME_SIZE));
  if(brightness == DARK){
    threshold(src, dst, 0, average);
  } else {
    threshold(src, dst, average, 255);
  }
}

void
fill_holes(uint8_t* src, uint8_t* dst, enum eConnected connected)
{
    register uint32_t width  = FRAME_WIDTH - 1;
    register uint32_t height = FRAME_HEIGHT - 1;
    register int32_t w;
    register int32_t h;
    register int32_t wf;
    register int32_t hf;
    register int32_t i;
    register uint32_t unfinished;
    uint8_t (*img)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&src[0];


    vCopy(src, dst); //<-- moet weg

    // Mark the border, exept the blobs
    //Horisontal
    for(w = FRAME_WIDTH; w >= 0; w--){
        if(img[0][w] == 0){
            img[0][w] = 2;
        }
        if(img[height][w] == 0){
            img[height][w] = 2;
        }
    }

    //Vertical
    for(h = FRAME_HEIGHT; h >= 0; h--){
        if(img[h][0] == 0){
            img[h][0] = 2;
        }
        if(img[h][width] == 0){
            img[h][width] = 2;
        }
    }

    /* Go over the rest of the image */
    do{
        //Mark the image form L->R and R->L
        w  = FRAME_WIDTH;
        h  = FRAME_HEIGHT;
        wf = 0;
        hf = 0;

        for(i = FRAME_SIZE; i > 0; i--){
            //Top left -> lower right
            if(img[hf][wf] == 0){
                if(neighbour_count(src, wf, hf, 2, connected) > 0){
                    img[hf][wf] = 2;
                }
            }

            //Lower right -> top left
            if(img[h][w] == 0){
                if(neighbour_count(src, w, h, 2, connected) > 0){
                    img[h][w] = 2;
                }
            }

            if(w == 0){
                w = FRAME_WIDTH;
                wf = 0;
                h--;
                hf++;
            } else {
                w--;
                wf++;
            }
        }

        //Check whether the blob is compleetly marked
        unfinished = 0;
        w = FRAME_WIDTH;
        h = FRAME_HEIGHT;

        for(i = FRAME_SIZE; i > 0; i--){
            if(img[h][w] == 0){
                if(neighbour_count(src, w, h, 2, connected) > 0){
                    unfinished = 1;
                    break;
                }
            }

            if(w == 0){
                w = FRAME_WIDTH;
                h--;
            } else {
                w--;
            }
        }
    }while(unfinished == 1);

    set_selected_to_value(img, img, 0, 1);
    set_selected_to_value(img,img, 2, 0);
}

void 
remove_border_blobs(uint8_t* src, uint8_t* dst, eConnected connected)
{
    register int32_t width  = FRAME_WIDTH - 1;
    register int32_t height = FRAME_HEIGHT - 1;
    register int32_t w;
    register int32_t h;
    register int32_t i;
    register uint32_t unfinished;
    register int32_t size = FRAME_SIZE - 1;
    uint8_t (*img)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&src[0];

    vCopy(src, dst); //<-- Moet weg

    /* Mark the border blobs */
    //Horisontal
    for(w = width; w >= 0; w--){
        img[0][w]      = img[0][w] * 2;
        img[height][w] = img[height][w] * 2;
    }

    //Vertical
    for(sh = height; sh >= 0; sh--){
        h = (uint32_t)(sh);
        img[h][0]     = img[h][0] * 2;
        img[h][width] = img[h][width] * 2;
    }

    /* Go over the rest of the image */
    do{
        w = width;
        h = height;

        //Mark the image form L->R and R->L
        for(i = size; i >= 0; i--){

            //Lower right -> top left
            if(src[i] == 1){
                if(neighbour_count(dst, w, h, 2, connected) > 0){
                    src[i] = 2;
                }
            }

            //Top left -> lower right
            if(src[size - i] == 1){
                if(neighbour_count(dst, (width - w), (height - h), 2, connected) > 0){
                    src[size - i] = 2;
                }
            }

            if(w == 0){
                w = width;
                h--;
            } else {
                w--;
            }
        }

        //Check whether the border blob is compleetly marked
        unfinished = 0;
        w = width;
        h = height;

        for(i = size; i >= 0; i--){

            if(src[i] == 1){
                if(neighbour_count(dst, w, h, 2, connected) > 0){
                    unfinished = 1;
                    break;
                }
            }

            if(w == 0){
                w = width;
                h--;
            } else {
                w--;
            }
        }
    }while(unfinished == 1);

    set_selected_to_value(src, dst, 2, 0);
}

uint32_t
label_blobs(uint8_t* src, uint8_t* dst, eConnected connected)
{
    register uint32_t blobCount = 1;
    register int32_t h;
    register int32_t w;
    register int32_t oh;
    register int32_t ow;
    register int32_t height = (int32_t)(FRAME_HEIGHT - 1);
    register int32_t width = (int32_t)(FRAME_WIDTH - 1);
    register int32_t i;
    register uint16_t j;
    register uint8_t foundFlag = 0;
    register uint8_t finished;
    register uint8_t lowest;
    register uint8_t blobDetected = 0;
    register uint8_t counter = 0;
    register int32_t size = (int32_t)(FRAME_SIZE - 1);
    register int32_t wp;
    register int32_t wm;
    register int32_t hp;
    register int32_t hm;
    uint8_t (*img)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&src[0];

    set_selected_to_value(src, src, 1, 255); //<-- modify threshold

    //Label the blobs
    w = 0;
    h = 0;

    for(i = size; i >= 0; i--){
        //If this pixel has to be labled
        if(img[h][w] == 255){
            blobDetected = true;
            if(neighbour_count(dst, w, h, blobCount, connected) > 0){ //If this blob is labeled
                img[h][w] = blobCount;
            } else { //This blob might be labeled with a lower number or is a new blob
                for(j = blobCount; j != 0; j--){ //Check what number the neighbour might be
                    if(neighbour_count(dst, w, h, j, connected) > 0){
                        foundFlag = true;
                        break;
                    }
                }

                if(foundFlag){ //If this pixel is part of a labeled blob, set the right number
                    img[h][w] = j;
                    foundFlag = false;
                } else { //Otherwise, label this as a new blob
                    blobCount += 1;
                    img[h][w] = blobCount;
                }

            }
        }

        if(w == width){
          w = 0;
          h++;
        } else {
          w++;
        }
    }

    if(blobDetected == false){
        return 0;
    }

    do{
        //Complete labeling the blobs from the lower right corner to upper left corner
        w = width;
        h = height;
        ow = 0;
        oh = 0;

        for(i = size; i >= 0; i--){

            if(img[h][w] > 0){
                lowest = img[h][w];

                //Check behind for lowest value with a exeption to 0
                if(h < FRAME_HEIGHT){
                    if(w < FRAME_WIDTH){
                        //Pre calculate
                        wp = w + 1;
                        hp = h + 1;

                        //
                        if((img[hp][wp] < lowest) && (img[hp][wp] > 0) && (connected == EIGHT)){
                            lowest = img[hp][wp];
                        }
                        if((img[h][wp] < lowest) && (img[h][wp] > 0)){
                            lowest = img[h][wp];
                        }
                    }
                    if((img[hp][w] < lowest) && (img[hp][w] > 0)){
                        lowest = img[hp][w];
                    }
                    if(w > 0){
                        wm = w - 1;

                        if((img[hp][wm] < lowest) && (img[hp][wm] > 0)  && (connected == EIGHT)){
                            lowest = img[hp][wm];
                        }
                    }
                }

                img[h][w] = lowest;
            }

            if(w == 0){
              w = width;
              ow = 0;
              h--;
              oh++;
            } else {
              w--;
              ow++;
            }
        }

        //Complete labeling the blobs from the lower upper left to right corner corner
        w = 0;
        h = 0;

        for(i = size; i >= 0; i--){

            if(img[h][w] > 0){
                lowest = img[h][w];

                //Check behind for lowest value with a exeption to 0
                if(h > 0){
                    if(w > 0){
                        //Pre-calculate
                        wm = w - 1;
                        hm = h - 1;

                        if((img[hm][wm] < lowest) && (img[hm][wm] > 0) && (connected == EIGHT)){
                            lowest = img[hm][wm];
                        }
                        if((img[h][wm] < lowest) && (img[h][wm] > 0)){
                            lowest = img[h][wm];
                        }
                    }
                    if((img[hm][w] < lowest) && (img[hm][w] > 0)){
                        lowest = img[hm][w];
                    }
                    if(w < dst->width){
                        //Pre-calculate
                        wp = w + 1;

                        if((img[hm][wp] < lowest) && (img[hm][wp] > 0) && (connected == EIGHT)){
                            lowest = img[hm][wp];
                        }
                    }
                }

                img[h][w] = lowest;
            }

            if(w == width){
              w = 0;
              h++;
            } else {
              w++;
            }
        }

        finished = 0;

        //Check whether 1 blob still has 2 labeles
        w = 0;
        h = 0;

        for(i = size; i >= 0; i--){
            if(img[h][w] > 0){
                if(neighbours_equal_or_higher(dst, w, h, (img[h][w] + 1), connected) > 0){
                    finished = 1;
                    break;
                }
            }

            if(w == width){
              w = 0;
              h++;
            } else {
              w++;
            }
        }
        
        counter++;
    }while(finished == 1 && counter < 10);

    //Re-label the blobs from 1 to x blobs
    blobCount = 0;
    w = 0;
    h = 0;

    for(i = size; i >= 0; i--){
        if(img[h][w] > 0){
            if(img[h][w] == (blobCount + 1)){
                //If current blob is in order of labeld blobs
                blobCount += 1;
            } else if(img[h][w] > (blobCount + 1)){
                //If current blob is more than 1 higher then previous blob, re-label
                blobCount += 1;
                set_selected_to_value(dst, dst, img[h][w], blobCount);
            }
        }

        if(w == width){
          w = 0;
          h++;
        } else {
          w++;
        }
    }

    return blobCount;
}

/**
  Adjust to our needs
 */
void 
vBlobAnalyse(image_t *img, uint8_t blobcount, blobinfo_t *pBlobInfo)
{
    /**
     * blobinfo_t consist of:
     * Blob height (pixels)
     * Blob width (pixels)
     * Blob number of pixels
     * Blob perimeter (omtrek in pixels, double)
     */
    register uint8_t b;
    register uint16_t w;
    register uint16_t h;
    uint16_t minWidth[32];
    uint16_t maxWidth[32];
    uint16_t minHeight[32];
    uint16_t maxHeight[32];
    
    if(blobcount > 32){
        blobcount = 32;
    }

    for(b = 0; b < blobs; b++){
        minWidth[b]             = FRAME_WIDTH;
        maxWidth[b]             = 0;
        minHeight[b]            = FRAME_HEIGHT;
        maxHeight[b]            = 0;
        pBlobInfo[b].height     = 0;
        pBlobInfo[b].width      = 0;
        pBlobInfo[b].nof_pixels = 0;
        pBlobInfo[b].perimeter  = 0;

        for(h = 0; h < img->height; h++){
            for(w = 0; w < img->width; w++){
                if(img->data[h][w] == (b + 1)){
                    //Calculate height
                    if(h < minHeight[b]){
                        minHeight[b] = h;
                    }
                    if(h > maxHeight[b]){
                        maxHeight[b] = h;
                    }
                    pBlobInfo[b].height = maxHeight[b] - minHeight[b];

                    //Calculate width
                    if(w < minWidth[b]){
                        minWidth[b] = w;
                    }
                    if(w > maxWidth[b]){
                        maxWidth[b] = w;
                    }
                    pBlobInfo[b].width = maxWidth[b] - minWidth[b];

                    //Count number of pixels
                    pBlobInfo[b].nof_pixels += 1;

                    //Calculate perimeter
                    if(neighbour_count(img, w, h, 0, FOUR) > 0){
                        if(neighbour_count(img, w, h, 0, FOUR) > 1){
                            if(neighbour_count(img, w, h, 0, FOUR) > 2){
                                pBlobInfo[b].perimeter += (double)(sqrt(5));
                            } else {
                                pBlobInfo[b].perimeter += (double)(sqrt(2));
                            }
                        } else {
                            pBlobInfo[b].perimeter += (double)(1);
                        }
                    }
                }
            }
        } 
    }
}

/******************************************************************************
 ************************** Localy used functions *****************************
 ******************************************************************************/

void 
threshold(uint8_t* src, uint8_t* dst, uint8_t low, uint8_t high)
{
  register uint8_t* srcPt = src;
  register uint8_t* dstPt = dst + FRAME_SIZE;

  for(srcPt = (src + FRAME_SIZE); srcPt >= src; srcPt--){
    if(*srcPt >= low && *srcPt <= high){
      dst[srcPt - src] = 1;
    } else {
      dst[srcPt - src]= 0;
    }
  }
}

void
histogram(uint8_t* src, uint16_t *hist, uint32_t *sum)
{
  register int32_t  i;
  register uint8_t* pixPt = src;

  *sum = 0;

  for(i = FRAME_SIZE; i > 0; i--){
      hist[*pixPt] += 1;
      *sum += *pixPt;
      pixPt++;
  }
}

uint8_t
neighbour_count(uint8_t* src, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected)
{
  register uint8_t nrOfNeighbours = 0;
  register uint8_t xMore = x > 0;
  register uint8_t xLess = x < FRAME_WIDTH;
  register uint8_t yMore = y > 0;
  register uint8_t yLess = y < FRAME_HEIGHT;
  register uint16_t xMin = x;
  register uint16_t xMax = x;
  register uint16_t yMin = y;
  register uint16_t yMax = y;
  uint8_t (*img)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&src[0];

  if(xMore){ //Left value
    xMin = x - 1;
    if(img[y][xMin] == value){
      nrOfNeighbours++;
    }
  }

  if(xLess){ //Right value
    xMax = x + 1;
    if(img[y][xMax] == value){
      nrOfNeighbours++;
    }
  }

  if(yMore){ //Upper value
    yMin = y - 1;
    if(img[yMin][x] == value){
      nrOfNeighbours++;
    }
  }

  if(yLess){ //Lower value
    yMax = y + 1;
    if(img[yMax][x] == value){
      nrOfNeighbours++;
    }
  }

  if(connected == EIGHT){
    if(xMore && yMore){ //Left upper value
      if(img[yMin][xMin] == value){
        nrOfNeighbours++;
      }
    }

    if(xLess && yMore){ //Right upper value
      if(img[yMin][xMax] == value){
        nrOfNeighbours++;
      }
    }

    if(xMore && yLess){ //Left lower value
      if(img[yMax][xMin] == value){
        nrOfNeighbours++;
      }
    }

    if(xLess && yLess){ //Right lower value
      if(img[yMax][xMax] == value){
        nrOfNeighbours++;
      }
    }
  }

  return nrOfNeighbours;
}

uint8_t
neighbours_equal_or_higher(uint8_t* src, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected)
{
  register uint8_t nrOfNeighbours = 0;
  register uint8_t xMore = x > 0;
  register uint8_t xLess = x < FRAME_WIDTH;
  register uint8_t yMore = y > 0;
  register uint8_t yLess = y < FRAME_HEIGHT;
  register uint16_t xMin = x;
  register uint16_t xMax = x;
  register uint16_t yMin = y;
  register uint16_t yMax = y;
  uint8_t (*img)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&src[0];

  if(xMore){ //Left value
    xMin = x - 1;
    if(img[y][xMin] >= value){
      nrOfNeighbours++;
    }
  }

  if(xLess){ //Right value
    xMax = x + 1;
    if(img[y][xMax] >= value){
      nrOfNeighbours++;
    }
  }

  if(yMore){ //Upper value
    yMin = y - 1;
    if(img[yMin][x] >= value){
      nrOfNeighbours++;
    }
  }

  if(yLess){ //Lower value
    yMax = y + 1;
    if(img[yMax][x] >= value){
      nrOfNeighbours++;
    }
  }

  if(connected >= EIGHT){
    if(xMore && yMore){ //Left upper value
      if(img[yMin][xMin] >= value){
        nrOfNeighbours++;
      }
    }

    if(xLess && yMore){ //Right upper value
      if(img[yMin][xMax] >= value){
        nrOfNeighbours++;
      }
    }

    if(xMore && yLess){ //Left lower value
      if(img[yMax][xMin] >= value){
        nrOfNeighbours++;
      }
    }

    if(xLess && yLess){ //Right lower value
      if(img[yMax][xMax] >= value){
        nrOfNeighbours++;
      }
    }
  }

  return nrOfNeighbours;
}

void
set_selected_to_value(uint8_t* src, uint8_t* dst, uint8_t selected, uint8_t value)
{
    register uint32_t* srcPt = (uint32_t*)(src);
    register uint32_t* dstPt = (uint32_t*)(dst);
    register uint32_t i;
    register uint32_t valueB1 = (uint32_t)(value);
    register uint32_t valueB2 = (uint32_t)(valueB1 << 8);
    register uint32_t valueB3 = (uint32_t)(valueB1 << 16);
    register uint32_t valueB4 = (uint32_t)(valueB1 << 24);
    register uint32_t selectedB1 = (uint32_t)(selected);
    register uint32_t selectedB2 = (uint32_t)(selectedB1 << 8);
    register uint32_t selectedB3 = (uint32_t)(selectedB1 << 16);
    register uint32_t selectedB4 = (uint32_t)(selectedB1 << 24);
    register uint32_t done = 0;
    register uint32_t temp;

    for(i = (FRAME_SIZE / 4); i > 0; i--){
        if((*srcPt & 0x000000FF) == selectedB1){
            temp = *srcPt & 0xFFFFFF00;
            temp |= valueB1;
            *dstPt = temp;
            done = 1;
        }
        if((*srcPt & 0x0000FF00) == selectedB2){
            temp = *srcPt & 0xFFFF00FF;
            temp |= valueB2;
            *dstPt = temp;
            done = 1;
        }
        if((*srcPt & 0x00FF0000) == selectedB3){
            temp = *srcPt & 0xFF00FFFF;
            temp |= valueB3;
            *dstPt = temp;
            done = 1;
        }
        if((*srcPt & 0xFF000000) == selectedB4){
            temp = *srcPt & 0x00FFFFFF;
            temp |= valueB4;
            *dstPt = temp;
            done = 1;
        }

        if(done == 0){
          *dstPt = *srcPt;
        } else {
          done = 0;
        }

        srcPt++;
        dstPt++;
    }
}
