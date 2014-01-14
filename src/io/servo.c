// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/pwm.h>
#include <autonerf/io/gpio.h>
#include <autonerf/io/servo.h>

void
servo_init(register const uint8_t io)
{
    pwm_init(io, 20000000);
    pwm_set_duty_cycle(io, SERVO_VALUE_MIN);
}

void
servo_set_position(register const uint8_t io, register const float position)
{
    uint32_t cycle = SERVO_VALUE_MIN + ((uint32_t) (position * (SERVO_DELTA_TIME / 180.0f)));

    if (cycle < SERVO_VALUE_MIN) { cycle = SERVO_VALUE_MIN; }
    if (cycle > SERVO_VALUE_MAX) { cycle = SERVO_VALUE_MAX; }

    LOG_INFO("Duty cycle for %f degrees: %d\n", position, cycle);

    pwm_set_duty_cycle(io, cycle);
}
