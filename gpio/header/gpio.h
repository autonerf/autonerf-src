/******************************************************************************
 * gpio.h                                                                     *
 *                                                                            *
 * Liberary to access the GPIO pins of the beaglebone.                        *
 * How to initialize the GPIO:                                                *
 * 1.) Calculate the GPIO variables. Can be done with initGPIO(chip, pin).    *
 * 2.) Export the GPIO pin with setGPIO(io).                                  *
 * 3.) Set a direction of the pin with setIO(io, direction).                  *
 *                                                                            *
 * Set a output with setOutput(io, value) where value is '1' or '0'.          *
 *                                                                            *
 * Read a output with getInput(io), a 1 or 0 will be returned.                *
 *****************************************************************************/

#ifndef GPIO
#define GPIO

/* Definitions */
#define PINPCHIP     32 //Pins per chip
#define INPUT_PIN  "in"
#define OUTPUT_PIN "low"

/* Prototypes */
void initGPIO(uint8_t* io, char* direction);
uint8_t calculateGPIO(uint8_t chip, uint8_t pin);
void unexportGPIO(uint8_t* io);
void setOutput(uint8_t* io, char value);
uint8_t getInput(uint8_t* io);

#endif
