#include <stdio.h>
#include <stdint.h>
#include "../header/gpio.h"
#include "../header/pwm.h"
#include "../header/servo.h"

#define MINVAL  520000
#define MAXVAL 1820000
#define DELTAT 1300000.0f

/**
 * Assignes a PWM GPIO to a servo.
 * @param io = the io number to assign
 */
void initServo(uint8_t* io)
{
    initPWM(io, 20000000);
    setDutyCycle(io, MINVAL);
}

/**
 * Writes the right angle to the servo.
 * @param io    = the servo IO pin to control
 * @param angle = the position in degrees
 */
void setPos(uint8_t* io, float angle)
{
    uint32_t dutyCycle;

    /* Caclucate the dutyCycle of the signal */
    dutyCycle = MINVAL + (uint32_t)(angle * (DELTAT / 180.0f));

    if(dutyCycle > MAXVAL){
        dutyCycle = MAXVAL;
    } else if(dutyCycle < MINVAL){
        dutyCycle = MINVAL;
    }

    printf("DutyCycle for %f degrees: %i\n", angle, dutyCycle);
    fflush(stdout);

    setDutyCycle(io, dutyCycle);
}

/**
 * Calculates the required GPIO values.
 * @param chip = the IO chip of the beaglebone
 * @param pin  = the IO pin of the chip of the beaglebone
 *
 * @return the stucture that defines the GPIO
 */
uint8_t calculateServo(uint8_t chip, uint8_t pin)
{
    return (chip * PINPCHIP) + pin;
}
