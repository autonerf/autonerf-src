/**
 * This file sets and controls GPIO pins
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gpio.h>

#define GPIOEXPORT   "/sys/class/gpio/export" //GPIO export file
#define GPIOUNEXPORT "/sys/class/gpio/unexport" //GPIO export file
#define GPIODIR      "/sys/class/gpio/gpio"
#define DIRECTION    "direction" //Direction file
#define VALUE        "value"

/* Local prototypes */
void exportGPIO(uint8_t* io);
void setIO(uint8_t* io, char* direction);

/**
 * Initializes a GPIO pin.
 * @param io        = the IO number that has to be assigned
 * @param direction = the direction of the IO
 */
void initGPIO(uint8_t* io, char* direction)
{
    exportGPIO(io);
    setIO(io, direction);
}

/**
 * Read a input pin.
 * @param io = the input variable
 */
uint8_t getInput(uint8_t* io)
{
    int32_t file = 0;
    char buff[3];
    char adress[100];

    //Construct adress line
    strcpy(adress, GPIODIR);
    sprintf(buff, "%i", *io);
    strcat(adress, buff);
    strcat(adress, "/");
    strcat(adress, VALUE);

    //Set GPIO direction
    if((file = open(adress, O_RDONLY)) != 0){
        read(file, buff, ( sizeof(buff) / sizeof(char) ));
        close(file);
    } else {
        printf("Faild to open GPIO value file. GPIO[%i]%i not set.\n\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
    }

    return (buff[0] - 48);
}

/**
 * Set a output to a sertan value.
 * @param io    = the GPIO to set
 * @param state = the new value of the output
 */
void setOutput(uint8_t* io, char state)
{
    int32_t file = 0;
    char buff[3];
    char adress[100];

    //Construct adress line
    strcpy(adress, GPIODIR);
    sprintf(buff, "%i", *io);
    strcat(adress, buff);
    strcat(adress, "/");
    strcat(adress, VALUE);

    //Set GPIO direction
    if((file = open(adress, O_WRONLY)) != 0){
        write(file, &state, ( sizeof(state) / sizeof(char) ));
        close(file);
    } else {
        printf("Faild to open GPIO value file. GPIO[%i]%i not set.\n\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
    }
}

/**
 * Un-sets a I/O to GPIO mode.
 * @param io = the GPIO to un-set from GPIO mode
 */
void unexportGPIO(uint8_t* io)
{
    int32_t file, length = 0;
    char buffer[4];

    length = sprintf(buffer, "%i", *io);

    //Unexport GPIO
    if((file = open(GPIOUNEXPORT, O_WRONLY)) != 0){
        write(file, buffer, length);
        close(file);
    } else {
        printf("Faild to open GPIO unexport file. GPIO[%i]%i not set.\n\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
    }
}

/**
 * Calculates the required GPIO values.
 * @param chip = the IO chip of the beaglebone
 * @param pin  = the IO pin of the chip of the beaglebone
 *
 * @return the stucture that defines the GPIO
 */
uint8_t calculateGPIO(uint8_t chip, uint8_t pin)
{
    return (chip * PINPCHIP) + pin;
}

/*******************************************************************************
 ******************************* LOCAL FUNCTIONS *******************************
 ******************************************************************************/

/**
 * Sets the GPIO as an output or intput.
 * @param io        = the GPIO to set
 * @param direction = set io as input or output
 */
void setIO(uint8_t* io, char* direction)
{
    int32_t file = 0;
    char buff[3];
    char adress[100];

    //Construct adress line
    strcpy(adress, GPIODIR);
    sprintf(buff, "%i", *io);
    strcat(adress, buff);
    strcat(adress, "/");
    strcat(adress, DIRECTION);

    //Set GPIO direction
    if((file = open(adress, O_WRONLY)) != 0){
        write(file, direction, ( sizeof(direction) / sizeof(char) ));
        close(file);
    } else {;
        printf("Faild to open GPIO direction file. GPIO[%i]%i not set.\n\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
    }
}

/**
 * Sets a I/O to GPIO mode.
 * @param io = the GPIO to set to GPIO mode
 */
void exportGPIO(uint8_t* io)
{
    int32_t file, length = 0;
    char buffer[4];

    length = sprintf(buffer, "%i", *io);

    //Export GPIO
    if((file = open(GPIOEXPORT, O_WRONLY)) != 0){
        write(file, buffer, length);
        close(file);
    } else {
        printf("Faild to open GPIO export file. GPIO[%i]%i not set.\n\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
    }
}
