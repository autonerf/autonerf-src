// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/pwm.h>
#include <autonerf/io/gpio.h>
#include <autonerf/io/servo.h>

void
servo_init(register const uint8_t io, register const uint32_t servo_min)
{
    pwm_init(io, 20000000);
    pwm_set_duty_cycle(io, servo_min);
}

void
servo_set_position(register const uint8_t io, register const float position, register const uint32_t servo_min, register const uint32_t servo_max)
{
    uint32_t cycle = servo_min + ((uint32_t) (position * (float)(servo_max - servo_min) / 180.0f));

    if (cycle < servo_min) { cycle = servo_min; }
    if (cycle > servo_max) { cycle = servo_max; }

    LOG_INFO("Duty cycle for %f degrees: %d", position, cycle);

    pwm_set_duty_cycle(io, cycle);
}
