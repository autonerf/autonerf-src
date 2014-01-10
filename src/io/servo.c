// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/pwm.h>
#include <autonerf/io/gpio.h>

void
servo_init(const register uint8_t io)
{
    pwm_init(io, 20000000);
    pwm_set_duty_cycle(io, SERVO_VALUE_MIN);
}

void
servo_set_position(const register uint8_t io, const register float position)
{
    uint32_t cycle = SERVO_VALUE_MIN + ((uint32_t) (position * (SERVO_DELTA_TIME / 180.0f)));

    if (cycle < SERVO_VALUE_MIN) { cycle = SERVO_VALUE_MIN; }
    if (cycle > SERVO_VALUE_MAX) { cycle = SERVO_VALUE_MAX; }

    LOG_INFO("Duty cycle for %f degrees: %d\n", position, cycle);

    pwm_set_duty_cycle(io, cycle);
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
