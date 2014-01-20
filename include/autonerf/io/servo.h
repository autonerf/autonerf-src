#ifndef AUTONERF_IO_SERVO_H
#define AUTONERF_IO_SERVO_H

// Standard C dependencies
#include <stdint.h>

/**
 Assign a PWM signal to a servo motor

 @param io The number of the I/O pin to assign
 @warning  *Warning:* The `io` parameter must be calculated using the `gpio_calculate` function
 @see      gpio_calculate
 */
extern void servo_init(register const uint8_t io, register const uint32_t servo_min);

/**
 Set a servo to a specific angle

 @param io    The servo I/O pin to use
 @param position The angle to set the servo to (in degrees)
 */
extern void servo_set_position(register const uint8_t io, register const float position, register const uint32_t servo_min, register const uint32_t servo_max);

#endif /* AUTONERF_IO_SERVO_H */
