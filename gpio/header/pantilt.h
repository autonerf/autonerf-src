/******************************************************************************
 * pantilt.h                                                                  *
 *                                                                            *
 * API to control the pan and tilt servo's.                                   *
 *****************************************************************************/

#ifndef PANTILT
#define PANTILT

/* Prototypes */
void initPanTilt(void);
void setPan(float angle);
void setTilt(float angle);
void setDeltaPan(float angle);
void setDeltaTilt(float angle);

#endif /* PANTILT */
