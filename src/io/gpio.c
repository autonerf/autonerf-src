// Standard C dependencies
#include <stdio.h>

// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/gpio.h>

/**
 * Sets the GPIO as an output or intput.
 * @param io        = the GPIO to set
 * @param direction = set io as input or output
 */
void
gpio_set_direction(register const uint8_t io, const char * direction)
{
    FILE *  file;
    char    filename[128];

    sprintf(filename, "%s%d/%s", GPIO_DIRECTORY, io, GPIO_FILE_DIRECTION);

    if ((file = fopen(filename, "w")) != NULL) {
        fprintf(file, "%s", direction);
        fclose(file);
    } else {
        LOG_ERROR(
            "Could not open GPIO direction file. GPIO[%d]%d not set.",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }
}

/**
 * Initializes a GPIO pin.
 * @param io        = the IO number that has to be assigned
 * @param direction = the direction of the IO
 */
void
gpio_init(register const uint8_t io, register const uint8_t direction)
{
    gpio_export(io);
    gpio_set_direction(io, (direction == 1) ? GPIO_PIN_INPUT : GPIO_PIN_OUTPUT);
}

/**
 * Sets a I/O to GPIO mode.
 * @param io = the GPIO to set to GPIO mode
 */
void
gpio_export(register const uint8_t io)
{
    FILE * file = fopen(GPIO_FILE_EXPORT, "w");

    if (file) {
        fprintf(file, "%d", io);
        fclose(file);
    } else {
        LOG_ERROR(
            "Failed to open GPIO export file: GPIO[%d]%d not set",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }
}

/**
 * Un-sets a I/O to GPIO mode.
 * @param io = the GPIO to un-set from GPIO mode
 */
void
gpio_unexport(register const uint8_t io)
{
    FILE * file = fopen(GPIO_FILE_UNEXPORT, "w");

    if (file) {
        fprintf(file, "%d", io);
        fclose(file);
    } else {
        LOG_ERROR(
            "Failed to open GPIO unexport file: GPIO[%d]%d not set",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }
}

void
gpio_write(register const uint8_t io, register const uint8_t value)
{
    FILE *  file;
    char    filename[128];

    sprintf(filename, "%s%d/%s", GPIO_DIRECTORY, io, GPIO_FILE_VALUE);

    if ((file = fopen(filename, "w")) != NULL) {
        fprintf(file, "%d", value);
        fclose(file);
    } else {
        LOG_ERROR(
            "Failed to open GPIO value file: GPIO[%d]%d not set",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }
}

uint8_t
gpio_read(register const uint8_t io)
{
    FILE *  file;
    int     result;
    char    filename[128];

    sprintf(filename, "%s%d/%s", GPIO_DIRECTORY, io, GPIO_FILE_VALUE);

    if ((file = fopen(filename, "r")) != NULL) {
        fscanf(file, "%d", &result);
        fclose(file);
    } else {
        LOG_ERROR(
            "Failed to open GPIO value file: GPIO[%d]%d not set",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }

    return (uint8_t) result;
}


/**
 * Calculates the required GPIO values.
 * @param chip = the IO chip of the beaglebone
 * @param pin  = the IO pin of the chip of the beaglebone
 *
 * @return the stucture that defines the GPIO
 */
uint8_t
gpio_calculate(register const uint8_t chip, register const uint8_t pin)
{
    return ((chip * GPIO_PIN_COUNT) + pin);
}
