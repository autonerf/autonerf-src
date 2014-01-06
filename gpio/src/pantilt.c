/**
 * This file controls the missiles
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../header/servo.h"
#include "../header/pantilt.h"

/* Structures */
struct servo_t {
    uint8_t  io;
    float    angle; //In degrees
};

/* Global variabels */
struct servo_t pan;
struct servo_t tilt;

/**
 * Initialises the pan and tilt servo's
 */
void initPanTilt(void)
{
   pan.io     = calculateServo(0, 22);
   tilt.io    = calculateServo(2, 16);
   pan.angle  = 0.0f;
   tilt.angle = 0.0f;

   initServo(&pan.io);
   initServo(&tilt.io);
}

/**
 * Adjusts the pan with the given angle.
 */
void setDeltaPan(float angle)
{
    pan.angle = pan.angle + angle;

    setPan(pan.angle);
}

/**
 * Adjusts the tilt with the given angle.
 */
void setDeltaTilt(float angle)
{
    tilt.angle = tilt.angle + angle;

    setTilt(pan.angle);
}

/**
 * Sets the pan to the given angle.
 */
void setPan(float angle)
{
    pan.angle = angle;
    
    if(pan.angle > 180.0f){
        pan.angle = 180.0f;
    } else if(pan.angle < 0.0f) {
        pan.angle = 0.0f;
    }

    setPos(&pan.io, pan.angle);
}

/**
 * Sets the tilt to the given angle.
 */
void setTilt(float angle)
{
    tilt.angle = angle;
    
    if(tilt.angle > 180.0f){
        tilt.angle = 180.0f;
    } else if(tilt.angle < 0.0f) {
        tilt.angle = 0.0f;
    }

    setPos(&tilt.io, tilt.angle);
}

