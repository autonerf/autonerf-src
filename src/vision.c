#include <autonerf/vision.h>

//Local prototypes
void threshold(uint8_t* img, uint8_t low, uint8_t high);
void histogram(uint8_t* img, uint16_t *hist, uint32_t *sum);
uint8_t neighbour_count(uint8_t* img, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
uint8_t neighbours_equal_or_higher(uint8_t* img, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
void set_selected_to_value(uint8_t* img, uint8_t selected, uint8_t value);

void
vision_process(uint8_t* img, uint16_t * pan, uint16_t * tilt)
{
    uint8_t blobs;
    uint32_t blob_pos_x = 0;
    uint32_t blob_pos_y = 0;
    pan = 0;
    tilt = 0;

    contrast_stretch_fast(img);
    threshold_iso_data(img, DARK);
    fill_holes(img, FOUR);
    remove_border_blobs(img, EIGHT);
    blobs = label_blobs(img, EIGHT);

    if(blobs > 0){
        blob_analyse(img, blobs, &blob_pos_x, &blob_pos_y);
    }
}

void
contrast_stretch_fast(uint8_t* img)
{
    register int32_t i;
    register uint8_t lPix = 255;
    register uint8_t hPix = 0;
    register float verhouding = 0.0f;
    register float lutVal;
    uint8_t lut[256];

    //Determin lowist and highest value
    for(i = (FRAME_SIZE - 1); i >= 0; i--){
        if(img[i] > hPix){
            hPix = img[i];
        }
        if(img[i] < lPix){
            lPix = img[i];
        }
    }

    if(lPix != hPix){
        verhouding = (255.0f) / ((float)(hPix - lPix));
    } else {
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

    //Create output file
    for(i = (FRAME_SIZE - 1); i >= 0; i--){
        img[i] = lut[img[i]];
    }
}

void
threshold_iso_data(uint8_t* img, enum eBrightness brightness)
{
  uint16_t hist[256];
  uint32_t som;
  register uint8_t average = 0;
  register int32_t  i;

  /* Set histogram to 0 */
  for(i = 255; i >= 0; i--){
      hist[i] = 0;
  }

  histogram(img, &hist[0], &som);

  /* Determin start point */
  for(i = 0; hist[i] > 0; i++){
    continue;
  }

  /* Calculate average */
  average = (uint8_t)(i + (int32_t)(som / FRAME_SIZE));
  if(brightness == DARK){
    threshold(img, 0, average);
  } else {
    threshold(img, average, 255);
  }
}

void
fill_holes(uint8_t* img, enum eConnected connected)
{
    register uint32_t width  = FRAME_WIDTH - 1;
    register uint32_t height = FRAME_HEIGHT - 1;
    register int32_t w;
    register int32_t h;
    register int32_t wf;
    register int32_t hf;
    register int32_t i;
    register uint32_t unfinished;
    uint8_t (*imgArr)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&img[0];

    // Mark the border, exept the blobs
    //Horisontal
    for(w = width; w >= 0; w--){
        if(imgArr[0][w] == 0){
            imgArr[0][w] = 2;
        }
        if(imgArr[height][w] == 0){
            imgArr[height][w] = 2;
        }
    }

    //Vertical
    for(h = height; h >= 0; h--){
        if(imgArr[h][0] == 0){
            imgArr[h][0] = 2;
        }
        if(imgArr[h][width] == 0){
            imgArr[h][width] = 2;
        }
    }

    /* Go over the rest of the image */
    do{
        //Mark the image form L->R and R->L
        w  = width;
        h  = height;
        wf = 0;
        hf = 0;

        for(i = FRAME_SIZE; i > 0; i--){
            //Top left -> lower right
            if(imgArr[hf][wf] == 0){
                if(neighbour_count(img, wf, hf, 2, connected) > 0){
                    imgArr[hf][wf] = 2;
                }
            }

            //Lower right -> top left
            if(imgArr[h][w] == 0){
                if(neighbour_count(img, w, h, 2, connected) > 0){
                    imgArr[h][w] = 2;
                }
            }

            if(w == 0){
                w = width;
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
        w = width;
        h = height;

        for(i = FRAME_SIZE; i > 0; i--){
            if(imgArr[h][w] == 0){
                if(neighbour_count(img, w, h, 2, connected) > 0){
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

    set_selected_to_value(img, 0, 1);
    set_selected_to_value(img, 2, 0);
}

void
remove_border_blobs(uint8_t* img, enum eConnected connected)
{
    register int32_t width  = FRAME_WIDTH - 1;
    register int32_t height = FRAME_HEIGHT - 1;
    register int32_t w;
    register int32_t h;
    register int32_t wf;
    register int32_t hf;
    register int32_t i;
    register uint32_t unfinished;
    register int32_t size = FRAME_SIZE - 1;
    uint8_t (*imgArr)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&img[0];

    /* Mark the border blobs */
    //Horisontal
    for(w = width; w >= 0; w--){
        imgArr[0][w]      = imgArr[0][w] * 2;
        imgArr[height][w] = imgArr[height][w] * 2;
    }

    //Vertical
    for(h = height; h >= 0; h--){
        imgArr[h][0]     = imgArr[h][0] * 2;
        imgArr[h][width] = imgArr[h][width] * 2;
    }

    /* Go over the rest of the image */
    do{
        w  = width;
        h  = height;
        wf = 0;
        hf = 0;

        //Mark the image form L->R and R->L
        for(i = size; i >= 0; i--){

            //Lower right -> top left
            if(img[i] == 1){
                if(neighbour_count(img, w, h, 2, connected) > 0){
                    img[i] = 2;
                }
            }

            //Top left -> lower right
            if(img[size - i] == 1){
                if(neighbour_count(img, wf, hf, 2, connected) > 0){
                    img[size - i] = 2;
                }
            }

            if(w == 0){
                w  = width;
                wf = 0;
                h--;
                hf++;
            } else {
                w--;
                wf++;
            }
        }

        //Check whether the border blob is compleetly marked
        unfinished = 0;
        w = width;
        h = height;

        for(i = size; i >= 0; i--){

            if(img[i] == 1){
                if(neighbour_count(img, w, h, 2, connected) > 0){
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

    set_selected_to_value(img, 2, 0);
}

uint32_t
label_blobs(uint8_t* img, enum eConnected connected)
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
    uint8_t (*imgArr)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&img[0];

    set_selected_to_value(img, 1, 255); //<-- modify threshold

    //Label the blobs
    w = 0;
    h = 0;

    for(i = size; i >= 0; i--){
        //If this pixel has to be labled
        if(imgArr[h][w] == 255){
            blobDetected = 1;
            if(neighbour_count(img, w, h, blobCount, connected) > 0){ //If this blob is labeled
                imgArr[h][w] = blobCount;
            } else { //This blob might be labeled with a lower number or is a new blob
                for(j = blobCount; j != 0; j--){ //Check what number the neighbour might be
                    if(neighbour_count(img, w, h, j, connected) > 0){
                        foundFlag = 1;
                        break;
                    }
                }

                if(foundFlag){ //If this pixel is part of a labeled blob, set the right number
                    imgArr[h][w] = j;
                    foundFlag = 0;
                } else { //Otherwise, label this as a new blob
                    blobCount += 1;
                    imgArr[h][w] = blobCount;
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

    if(blobDetected == 0){
        return 0;
    }

    do{
        //Complete labeling the blobs from the lower right corner to upper left corner
        w = width;
        h = height;
        ow = 0;
        oh = 0;

        for(i = size; i >= 0; i--){

            if(imgArr[h][w] > 0){
                lowest = imgArr[h][w];

                //Check behind for lowest value with a exeption to 0
                if(h < FRAME_HEIGHT){
                    if(w < FRAME_WIDTH){
                        //Pre calculate
                        wp = w + 1;
                        hp = h + 1;

                        //
                        if((imgArr[hp][wp] < lowest) && (imgArr[hp][wp] > 0) && (connected == EIGHT)){
                            lowest = imgArr[hp][wp];
                        }
                        if((imgArr[h][wp] < lowest) && (imgArr[h][wp] > 0)){
                            lowest = imgArr[h][wp];
                        }
                    }
                    if((imgArr[hp][w] < lowest) && (imgArr[hp][w] > 0)){
                        lowest = imgArr[hp][w];
                    }
                    if(w > 0){
                        wm = w - 1;

                        if((imgArr[hp][wm] < lowest) && (imgArr[hp][wm] > 0)  && (connected == EIGHT)){
                            lowest = imgArr[hp][wm];
                        }
                    }
                }

                imgArr[h][w] = lowest;
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

            if(imgArr[h][w] > 0){
                lowest = imgArr[h][w];

                //Check behind for lowest value with a exeption to 0
                if(h > 0){
                    if(w > 0){
                        //Pre-calculate
                        wm = w - 1;
                        hm = h - 1;

                        if((imgArr[hm][wm] < lowest) && (imgArr[hm][wm] > 0) && (connected == EIGHT)){
                            lowest = imgArr[hm][wm];
                        }
                        if((imgArr[h][wm] < lowest) && (imgArr[h][wm] > 0)){
                            lowest = imgArr[h][wm];
                        }
                    }
                    if((imgArr[hm][w] < lowest) && (imgArr[hm][w] > 0)){
                        lowest = imgArr[hm][w];
                    }
                    if(w < FRAME_WIDTH){
                        //Pre-calculate
                        wp = w + 1;

                        if((imgArr[hm][wp] < lowest) && (imgArr[hm][wp] > 0) && (connected == EIGHT)){
                            lowest = imgArr[hm][wp];
                        }
                    }
                }

                imgArr[h][w] = lowest;
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
            if(imgArr[h][w] > 0){
                if(neighbours_equal_or_higher(img, w, h, (imgArr[h][w] + 1), connected) > 0){
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
        if(imgArr[h][w] > 0){
            if(imgArr[h][w] == (blobCount + 1)){
                //If current blob is in order of labeld blobs
                blobCount += 1;
            } else if(imgArr[h][w] > (blobCount + 1)){
                //If current blob is more than 1 higher then previous blob, re-label
                blobCount += 1;
                set_selected_to_value(img, imgArr[h][w], blobCount);
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
blob_analyse(uint8_t *img, uint8_t blobcount, uint32_t* blob_pos_x, uint32_t* blob_pos_y)
{
     //blobinfo_t consist of:
     //Blob height (pixels)
     //Blob width (pixels)
     //Blob number of pixels

    register int32_t i;
    register uint32_t width = (FRAME_WIDTH - 1);
    register uint32_t height = (FRAME_HEIGHT - 1);
    register uint32_t w = width;
    register uint32_t h = height;
    uint32_t blob_mass[255];
    uint32_t largest_blob = 0;
    uint32_t min_width = width;
    uint32_t max_width = 0;
    uint32_t min_height = height;
    uint32_t max_height = 0;
    uint8_t (*imgArr)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&img[0];

    //Clear blob_mass registers
    for(i = 255; i >= 0; i--){
        blob_mass[i] = 0;
    }

    //Calculate blob mass
    for(i = (FRAME_SIZE - 1); i >= 0; i--){
        if(img[i] > 0){
            blob_mass[img[i]]++;
        }
    }

    //Search for largest blob
    for(i = blobcount; i > 0; i--){
        if(blob_mass[i] > largest_blob){
            largest_blob = blob_mass[i];
        }
    }

    //Get coordinates
    for(i = (FRAME_SIZE - 1); i >= 0; i--){
        if(imgArr[h][w] == largest_blob){
            //Calculate height
            if(h < min_height){
                min_height = h;
            }
            if(h > max_height){
                max_height = h;
            }

            //Calculate width
            if(w < min_width){
                min_width = w;
            }
            if(w > max_width){
                max_width = w;
            }
        }

        if(w == width){
          w = 0;
          h--;
        } else {
          w--;
        }
    }

    *blob_pos_y = (max_height - min_height) / 2;
    *blob_pos_x = (max_width - min_width) / 2;
}

/******************************************************************************
 ************************** Localy used functions *****************************
 ******************************************************************************/

void
threshold(uint8_t* img, uint8_t low, uint8_t high)
{
    size_t i = 0;

    for (; i < FRAME_SIZE; i++) {
        img[i] = (img[i] >= low && img[i] <= high);
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
set_selected_to_value(uint8_t* img, uint8_t selected, uint8_t value)
{
    register uint32_t* srcPt = (uint32_t*)(img);
    register uint32_t i;
    register uint32_t valueB1 = (uint32_t)(value);
    register uint32_t valueB2 = (uint32_t)(valueB1 << 8);
    register uint32_t valueB3 = (uint32_t)(valueB1 << 16);
    register uint32_t valueB4 = (uint32_t)(valueB1 << 24);
    register uint32_t selectedB1 = (uint32_t)(selected);
    register uint32_t selectedB2 = (uint32_t)(selectedB1 << 8);
    register uint32_t selectedB3 = (uint32_t)(selectedB1 << 16);
    register uint32_t selectedB4 = (uint32_t)(selectedB1 << 24);
    register uint32_t temp;

    for(i = (FRAME_SIZE / 4); i > 0; i--){
        if((*srcPt & 0x000000FF) == selectedB1){
            temp = *srcPt & 0xFFFFFF00;
            temp |= valueB1;
            *srcPt = temp;
        }
        if((*srcPt & 0x0000FF00) == selectedB2){
            temp = *srcPt & 0xFFFF00FF;
            temp |= valueB2;
            *srcPt = temp;
        }
        if((*srcPt & 0x00FF0000) == selectedB3){
            temp = *srcPt & 0xFF00FFFF;
            temp |= valueB3;
            *srcPt = temp;
        }
        if((*srcPt & 0xFF000000) == selectedB4){
            temp = *srcPt & 0x00FFFFFF;
            temp |= valueB4;
            *srcPt = temp;
        }

        srcPt++;
    }
}
