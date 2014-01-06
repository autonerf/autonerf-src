/******************************************************************************
 * servo.h                                                                    *
 *                                                                            *
 * API to control a servo via a hardware PWM.                                 *
 *****************************************************************************/

#ifndef SERVO
#define SERVO

/* Prototypes */
void initServo(uint8_t* io);
void setPos(uint8_t* io, float angle);
uint8_t calculateServo(uint8_t chip, uint8_t pin);

#endif /* SERVO */
