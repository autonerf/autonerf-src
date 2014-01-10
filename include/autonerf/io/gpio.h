#ifndef AUTONERF_IO_GPIO_H
#define AUTONERF_IO_GPIO_H

// Standard C dependencies
#include <stdint.h>

// Defines
#define GPIO_PIN_COUNT      32
#define GPIO_PIN_INPUT      "in"
#define GPIO_PIN_OUTPUT     "out"
#define GPIO_FILE_EXPORT    "/sys/class/gpio/export"
#define GPIO_FILE_UNEXPORT  "/sys/class/gpio/unexport"
#define GPIO_DIRECTORY      "/sys/class/gpio/gpio"
#define GPIO_FILE_VALUE     "value"
#define GPIO_FILE_DIRECTION "direction"

/**
 Initialize a GPIO pin as a certain I/O pint with a certain direction.

 @param io        The I/O pin to initialize
 @param direction The direction of the GPIO pin (either `1` for input or `0` for output)
 @warning         *Warning:* The `io` parameter must be calculated using the `gpio_calculate` function
 @see             gpio_calculate
 */
extern void gpio_init(const register uint8_t io, const register gpio_direction_t direction);

/**
 Set an I/O pin to GPIO mode

 @param io The I/O pin to set to GPIO mode
 */
extern void gpio_export(const register uint8_t io);

/**
 Remove an I/O pin from GPIO mode

 @param io The I/O pin to remove from GPIO mode
 */
extern void gpio_unexport(const register uint8_t io);

/**
 Write a value to an GPIO pin

 @param io    The I/O pin to write
 @param value The value to write
 */
extern void gpio_write(const register uint8_t io, const register uint8_t value);

/**
 Read a value from a GPIO pin

 @param io The I/O pin to read
 @return   The value of the I/O pin
 */
extern uint8_t gpio_read(const register uint8_t io);

/**
 Calculate the pin number of a pin on a certain I/O chip

 @param chip The chip to use
 @param pin  The pin on the chip that has to be used
 @return     The pin number
 */
extern uint8_t gpio_calculate(const register uint8_t chip, const register uint8_t pin);

#endif /* AUTONERF_IO_GPIO_H */
