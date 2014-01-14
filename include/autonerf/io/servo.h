#ifndef AUTONERF_IO_SERVO_H
#define AUTONERF_IO_SERVO_H

// Standard C dependencies
#include <stdint.h>

// Defines
#define SERVO_VALUE_MIN     520000
#define SERVO_VALUE_MAX     1820000
#define SERVO_DELTA_TIME    1300000.0f

/**
 Assign a PWM signal to a servo motor

 @param io The number of the I/O pin to assign
 @warning  *Warning:* The `io` parameter must be calculated using the `gpio_calculate` function
 @see      gpio_calculate
 */
extern void servo_init(const register uint8_t io);

/**
 Set a servo to a specific angle

 @param io    The servo I/O pin to use
 @param angle The angle to set the servo to (in degrees)
 */
extern void servo_set_position(const register uint8_t io, const register float angle);

#endif /* AUTONERF_IO_SERVO_H */