#include <stdio.h>
#include <autonerf/vision.h>

//Local prototypes
void threshold(uint8_t* img, uint8_t low, uint8_t high);
void histogram(uint8_t* img, uint16_t *hist, uint32_t *sum);
uint8_t neighbour_count(struct frame_t * frame, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
uint8_t neighbours_equal_or_higher(struct frame_t * frame, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected);
void set_selected_to_value(uint8_t* img, uint8_t selected, uint8_t value);

void
erode(struct frame_t * frame, uint8_t thres, enum eConnected connected)
{
    register uint32_t x = 0;
    register uint32_t y = 0;

    for (y = 0; y < FRAME_HEIGHT; y++) {
        for (x = 0; x < FRAME_WIDTH; x++) {
            if (frame->grayscale[y][x]) {
                if (neighbour_count(frame, x, y, frame->grayscale[y][x], connected) < thres) {
                    frame->grayscale[y][x] = 0;
                }
            }
        }
    }
}

void
vision_process(uint8_t* img, uint16_t * pan, uint16_t * tilt)
{
    // uint8_t blobs;
    // uint32_t blob_pos_x = 0;
    // uint32_t blob_pos_y = 0;
    // pan = 0;
    // tilt = 0;

    // contrast_stretch_fast(img);
    // threshold_iso_data(img, DARK);
    // // fill_holes(img, FOUR);
    // remove_border_blobs(img, EIGHT);
    // blobs = label_blobs(img, EIGHT);

    // if(blobs > 0){
    //     blob_analyse(img, blobs, &blob_pos_x, &blob_pos_y);
    // }
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
threshold_iso_data(uint8_t * img, enum eBrightness brightness)
{
  uint16_t hist[256] = {0};
  uint32_t som;
  register uint32_t i;
  register uint32_t T;    //T[i]
  register uint32_t TiMO; //T[i - 1]
  register uint32_t MAT;
  register uint32_t MBT;
  register uint32_t noPixL;
  register uint32_t noPixR;
  register uint32_t somL;
  register uint32_t somR;

  histogram(img, &hist[0], &som);

  //Step 1: Compute mean from histogram; T
  for(i = 0; hist[i] > 0; i++){
    continue;
  }

  T = i + (som / (FRAME_WIDTH * FRAME_HEIGHT));

  //Step 2: Compute mean above T (MAT) and mean below T (MBT)
  do{
      noPixL = 0;
      noPixR = 0;
      somL   = 0;
      somR   = 0;
      TiMO = T;

      //Left values (including threshold mean)
      for(i = (T + 1); i > 0; i--){
        somL += hist[i - 1] * (i - 1);
        noPixL += hist[i - 1];
      }
      //Right values (excluding threshold mean)
      for(i = 255; i > T; i--){
        somR += hist[i] * i;
        noPixR += hist[i];
      }

      if(noPixL != 0){
          MBT = (uint32_t)(somL / noPixL);
      } else {
          MBT = 0;
      }
      if(noPixR != 0){
          MAT = (uint32_t)(somR / noPixR);
      } else {
          MAT = 0;
      }

  //Step 3: Compute mean; repeat step 2 + 3 if T[i] != T[i-1]
      T = (MAT + MBT) / 2;
  }while(T != TiMO);

  //Threshold
  if(brightness == DARK){
    threshold(img, 0, T);
  } else {
    threshold(img, T, 255);
  }
}

void
fill_holes(struct frame_t * frame, enum eConnected connected)
{
    register uint32_t x    = 0;
    register uint32_t y    = 0;
    register uint8_t  done = 0;

    // Mark horizontal and vertical borders
    for (x = 0; x < FRAME_WIDTH; x++) {
        if (frame->grayscale[0][x] == 0) {
            frame->grayscale[0][x] = 2;
        }
        if (frame->grayscale[FRAME_HEIGHT - 1][x] == 0) {
            frame->grayscale[FRAME_HEIGHT - 1][x] = 2;
        }
    }

    for (y = 0; y < FRAME_HEIGHT; y++) {
        if (frame->grayscale[y][0] == 0) {
            frame->grayscale[y][0] = 2;
        }
        if (frame->grayscale[y][FRAME_WIDTH - 1] == 0) {
            frame->grayscale[y][FRAME_WIDTH - 1] = 2;
        }
    }

    // Mark pixels that are not part of a blob but are connected to the border
    while (!done) {
        done = 1;

        for (y = 0; y < FRAME_HEIGHT; y++) {
            for (x = 0; x < FRAME_WIDTH; x++) {
                if (!frame->grayscale[y][x] && neighbour_count(frame, x, y, 2, connected) > 0) {
                    frame->grayscale[y][x] = 2;
                }
            }
        }

        for (y = FRAME_HEIGHT; y > 0; y--) {
            for (x = FRAME_WIDTH; x > 0; x--) {
                if (!frame->grayscale[(y - 1)][(x - 1)] && neighbour_count(frame, (x - 1), (y - 1), 2, connected) > 0) {
                    frame->grayscale[(y - 1)][(x - 1)] = 2;
                }
            }
        }

        for (y = 0; y < FRAME_HEIGHT; y++) {
            for (x = 0; x < FRAME_WIDTH; x++) {
                if (!frame->grayscale[y][x] && neighbour_count(frame, x, y, 2, connected) > 0) {
                    done                   = 0;
                    frame->grayscale[y][x] = 2;
                }
            }
        }
    }

    set_selected_to_value((uint8_t *) frame->grayscale, 0, 1);
    set_selected_to_value((uint8_t *) frame->grayscale, 2, 0);
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
                // if(neighbour_count(img, w, h, 2, connected) > 0){
                    img[i] = 2;
                // }
            }

            //Top left -> lower right
            if(img[size - i] == 1){
                // if(neighbour_count(img, wf, hf, 2, connected) > 0){
                    img[size - i] = 2;
                // }
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
                // if(neighbour_count(img, w, h, 2, connected) > 0){
                    unfinished = 1;
                    break;
                // }
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
label_blobs(struct frame_t * frame, enum eConnected connected)
{
    register uint32_t x       = 0;
    register uint32_t y       = 0;
    register uint32_t i       = 0;
    register uint32_t dx      = 0;
    register uint32_t dy      = 0;
    register uint32_t count   = 0;
    register uint8_t  changed = 1;

    set_selected_to_value((uint8_t *) frame->grayscale, 1, 255);

    while (changed) {
        changed = 0;

        for (y = 0; y < FRAME_HEIGHT; y++) {
            for (x = 0; x < FRAME_WIDTH; x++) {
                if (frame->grayscale[y][x]) {
                    register uint8_t lowest = frame->grayscale[y][x];

                    if (x > 0 && frame->grayscale[y][x - 1] && frame->grayscale[y][x - 1] < lowest) {
                        lowest = frame->grayscale[y][x - 1];
                    }
                    if (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] && frame->grayscale[y][x + 1] < lowest) {
                        lowest = frame->grayscale[y][x + 1];
                    }
                    if (y > 0 && frame->grayscale[y - 1][x] && frame->grayscale[y - 1][x] < lowest) {
                        lowest = frame->grayscale[y - 1][x];
                    }
                    if (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] && frame->grayscale[y + 1][x] < lowest) {
                        lowest = frame->grayscale[y + 1][x];
                    }
                    if (connected == EIGHT) {
                        if (x > 0 && y > 0 &&
                            frame->grayscale[y - 1][x - 1] &&
                            frame->grayscale[y - 1][x - 1] < lowest) {
                            lowest = frame->grayscale[y - 1][x - 1];
                        }
                        if (x < (FRAME_WIDTH - 1) && y > 0 &&
                            frame->grayscale[y - 1][x + 1] &&
                            frame->grayscale[y - 1][x + 1] < lowest) {
                            lowest = frame->grayscale[y - 1][x + 1];
                        }
                        if (x < (FRAME_WIDTH - 1) && y < (FRAME_HEIGHT - 1) &&
                            frame->grayscale[y + 1][x + 1] &&
                            frame->grayscale[y + 1][x + 1] < lowest) {
                            lowest = frame->grayscale[y + 1][x + 1];
                        }
                        if (x > 0 && y < (FRAME_HEIGHT - 1) &&
                            frame->grayscale[y + 1][x - 1] &&
                            frame->grayscale[y + 1][x - 1] < lowest) {
                            lowest = frame->grayscale[y + 1][x - 1];
                        }
                    }
                    if (lowest < frame->grayscale[y][x]) {
                        changed                 = 1;
                        frame->grayscale[y][x]  = lowest;
                    } else if (frame->grayscale[y][x] == 255) {
                        changed                 = 1;
                        frame->grayscale[y][x]  = count++;
                    }
                }
            }
        }

        for (dy = FRAME_HEIGHT; dy > 0; dy--) {
            for (dx = FRAME_WIDTH; dx > 0; dx--) {
                y = dy - 1;
                x = dx - 1;

                if (frame->grayscale[y][x]) {
                    register uint8_t lowest = frame->grayscale[y][x];

                    if (x > 0 && frame->grayscale[y][x - 1] && frame->grayscale[y][x - 1] < lowest) {
                        lowest = frame->grayscale[y][x - 1];
                    }
                    if (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] && frame->grayscale[y][x + 1] < lowest) {
                        lowest = frame->grayscale[y][x + 1];
                    }
                    if (y > 0 && frame->grayscale[y - 1][x] && frame->grayscale[y - 1][x] < lowest) {
                        lowest = frame->grayscale[y - 1][x];
                    }
                    if (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] && frame->grayscale[y + 1][x] < lowest) {
                        lowest = frame->grayscale[y + 1][x];
                    }
                    if (connected == EIGHT) {
                        if (x > 0 && y > 0 &&
                            frame->grayscale[y - 1][x - 1] &&
                            frame->grayscale[y - 1][x - 1] < lowest) {
                            lowest = frame->grayscale[y - 1][x - 1];
                        }
                        if (x < (FRAME_WIDTH - 1) && y > 0 &&
                            frame->grayscale[y - 1][x + 1] &&
                            frame->grayscale[y - 1][x + 1] < lowest) {
                            lowest = frame->grayscale[y - 1][x + 1];
                        }
                        if (x < (FRAME_WIDTH - 1) && y < (FRAME_HEIGHT - 1) &&
                            frame->grayscale[y + 1][x + 1] &&
                            frame->grayscale[y + 1][x + 1] < lowest) {
                            lowest = frame->grayscale[y + 1][x + 1];
                        }
                        if (x > 0 && y < (FRAME_HEIGHT - 1) &&
                            frame->grayscale[y + 1][x - 1] &&
                            frame->grayscale[y + 1][x - 1] < lowest) {
                            lowest = frame->grayscale[y + 1][x - 1];
                        }
                    }
                    if (lowest < frame->grayscale[y][x]) {
                        changed                 = 1;
                        frame->grayscale[y][x]  = lowest;
                    } else if (frame->grayscale[y][x] == 255) {
                        changed                 = 1;
                        frame->grayscale[y][x]  = count++;
                    }
                }
            }
        }
    }

    count = 0;

    for (y = 0; y < FRAME_HEIGHT; y++) {
        for (x = 0; x < FRAME_WIDTH; x++) {
            if (frame->grayscale[y][x] && frame->grayscale[y][x] > count) {
                set_selected_to_value((uint8_t *) frame->grayscale, frame->grayscale[y][x], ++count);
            }
        }
    }

    return count;
}

/**
  Adjust to our needs
 */
void
blob_analyse(struct frame_t * frame, uint8_t count, int32_t * position)
{
     //blobinfo_t consist of:
     //Blob height (pixels)
     //Blob width (pixels)
     //Blob number of pixels

    // register int32_t i;
    // register uint32_t width = (FRAME_WIDTH - 1);
    // register uint32_t height = (FRAME_HEIGHT - 1);
    // register uint32_t w = width;
    // register uint32_t h = height;
    // uint32_t blob_mass[255];
    // uint32_t largest_blob = 0;
    // uint32_t min_width = width;
    // uint32_t max_width = 0;
    // uint32_t min_height = height;
    // uint32_t max_height = 0;
    // uint8_t (*imgArr)[FRAME_HEIGHT] = (uint8_t (*)[FRAME_HEIGHT])&img[0];

    uint8_t masses[255];
    register int i              = -1;
    register uint32_t x         = 0;
    register uint32_t y         = 0;
    register uint32_t largest   = 0;
    register uint16_t min[2]    = {FRAME_WIDTH, FRAME_HEIGHT};
    register uint16_t max[2]    = {0, 0};

    for (i = 0; i < count; i++) {
        masses[i] = 0;
    }

    for (y = 0; y < FRAME_HEIGHT; y++) {
        for (x = 0; x < FRAME_WIDTH; x++) {
            if (frame->grayscale[y][x]) {
                masses[frame->grayscale[y][x]]++;
            }
        }
    }

    for (i = 1; i < count; i++) {
        if (masses[i] > largest) {
            largest = i;
        }
    }

    if (largest == -1) {
        position[0] = -1;
        position[1] = -1;
        return;
    }


    for (y = 0; y < FRAME_HEIGHT; y++) {
        for (x = 0; x < FRAME_WIDTH; x++) {
            if (frame->grayscale[y][x] == largest) {
                if (x < min[0]) { min[0] = x; }
                if (x > max[0]) { max[0] = x; }
                if (y < min[1]) { min[1] = y; }
                if (y > max[1]) { max[1] = y; }
            }
        }
    }

    // printf("X: (%d, %d)\n", min[0], max[0]);
    // printf("Y: (%d, %d)\n", min[1], max[1]);

    if ((max[0] - min[0]) && (max[1] - min[1])) {
        position[0] = ((max[0] - min[0]) / 2 + min[0]);
        position[1] = ((max[1] - min[1]) / 2 + min[1]);
    } else {
        position[0] = -1;
        position[1] = -1;
    }
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
neighbour_count(struct frame_t * frame, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected)
{
    if (connected == FOUR) {
        return ((x > 0 && frame->grayscale[y][x - 1] == value) +
                (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] == value) +
                (y > 0 && frame->grayscale[y - 1][x] == value) +
                (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] == value));
    }

    return ((x > 0 && frame->grayscale[y][x - 1] == value) +
            (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] == value) +
            (y > 0 && frame->grayscale[y - 1][x] == value) +
            (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] == value) +
            (x > 0 && y > 0 && frame->grayscale[y - 1][x - 1] == value) +
            (x < (FRAME_WIDTH - 1) && y > 0 && frame->grayscale[y - 1][x + 1] == value) +
            (x < (FRAME_WIDTH - 1) && y < (FRAME_HEIGHT - 1) && frame->grayscale[x + 1][y + 1] == value) +
            (x > 0 && y < (FRAME_HEIGHT - 1) && frame->grayscale[x - 1][y + 1] == value));
}

uint8_t
neighbours_equal_or_higher(struct frame_t * frame, uint16_t x, uint16_t y, uint8_t value, enum eConnected connected)
{
    if (connected == FOUR) {
        return ((x > 0 && frame->grayscale[y][x - 1] >= value) +
                (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] >= value) +
                (y > 0 && frame->grayscale[y - 1][x] >= value) +
                (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] >= value));
    }

    return ((x > 0 && frame->grayscale[y][x - 1] >= value) +
            (x < (FRAME_WIDTH - 1) && frame->grayscale[y][x + 1] >= value) +
            (y > 0 && frame->grayscale[y - 1][x] >= value) +
            (y < (FRAME_HEIGHT - 1) && frame->grayscale[y + 1][x] >= value) +
            (x > 0 && y > 0 && frame->grayscale[y - 1][x - 1] >= value) +
            (x < (FRAME_WIDTH - 1) && y > 0 && frame->grayscale[y - 1][x + 1] >= value) +
            (x < (FRAME_WIDTH - 1) && y < (FRAME_WIDTH - 1) && frame->grayscale[x + 1][y + 1] >= value) +
            (x > 0 && y < (FRAME_WIDTH - 1) && frame->grayscale[x - 1][y + 1] >= value));
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
