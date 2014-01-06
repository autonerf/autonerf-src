/**
 * This file controls PWM on PWM pins.
 * !! ONLY FOR EHRPWM pins !!
 * EHR  = Enhanced High Resolution
 * ECAP = Enhanced capture
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../header/pwm.h"
#include "../header/gpio.h"

#define PWMDIR "/sys/devices/ocp.2/pwm_test_P" //Complete in code
#define PERIOD "period"
#define DUTY   "duty"

/* Structures */
typedef struct {
    uint8_t  header;
    uint8_t  hpin;
    uint8_t  pwmPin;
    uint8_t  suffix;
    uint8_t  io;
    uint32_t period;
}PWMstr;

/* Local prototypes */
uint8_t checkPin(uint8_t* io);
void setFrequency(uint8_t* io, uint32_t* period);
void writePWMFile(PWMstr* pinInfo, uint32_t value, char* fileName);

/* Local global variables */
PWMstr channal[NOCHANNALS] = {
                                {0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 0}
                             };

/**
 * Initializes the PWM period cycle on the pin.
 *
 * @param chip   = the chip the pin is on
 * @param pin    = the pin of the chip
 * @param period = the period of the PWM signal in nano seconds
 */
void initPWM(uint8_t* io, uint32_t period)
{
    //Check whether io is a PWM pin
    if(checkPin(io) == 0){
        printf("Faild to assign PWM pin. GPIO[%i]%i is not a PWM pin!\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
        return;
    }

    //Set the frequency
    setFrequency(io, &period);
    setDutyCycle(io, 0);
}

/**
 * Writes a value to a PWM pin.
 * Interface to uint8_t instead of PWMstr*!
 */
void setDutyCycle(uint8_t* io, uint32_t newValue)
{
    uint8_t i;

    for(i = 0;i < NOCHANNALS ;i++){
        if(channal[i].io == *io){
            break;
        }
    }

    if(i > NOCHANNALS){
        printf("GPIO[%i]%i is not a PWM pin or is not initialized.\n", (*io / PINPCHIP), (*io % PINPCHIP));
        fflush(stdout);
        return;
    }

    //newValue = channal[i].period - newValue;
    writePWMFile(&channal[i], newValue, DUTY);
}

/**
 * Simple tool to convert the frequency the required period time.
 * @param frequency = frequency in Hz
 *
 * @return the period time in nano seconds
 */
uint32_t freqToPeriod(uint32_t frequency)
{
    return (uint32_t)(1000000000 / frequency);
}

/**
 * Calculates the required GPIO values.
 * @param chip = the IO chip of the beaglebone
 * @param pin  = the IO pin of the chip of the beaglebone
 *
 * @return the value that represents the GPIO
 */
uint8_t calculatePWM(uint8_t chip, uint8_t pin)
{
    return (chip * PINPCHIP) + pin;
}

/*******************************************************************************
 ******************************* LOCAL FUNCTIONS *******************************
 ******************************************************************************/

/**
 * A 
 */
void setFrequency(uint8_t* io, uint32_t* period)
{
    uint8_t pinValP8[NOCHANNALS] = { 23,  22,  81,  80,  70,  71};
    uint8_t pinPosP8[NOCHANNALS] = { 12,  19,  34,  36,  45,  46};
    uint8_t pinPwmP8[NOCHANNALS] = {  6,   5,   4,   3,   5,   6};
    uint8_t pinSufP8[NOCHANNALS] = {  0,  10,   0,  11,   0,   0}; //Check system after system set

    uint8_t pinValP9[NOCHANNALS] = { 50,  51,   3,   2, 111, 110};
    uint8_t pinPosP9[NOCHANNALS] = { 14,  16,  21,  22,  29,  31};
    uint8_t pinPwmP9[NOCHANNALS] = {  3,   4,   2,   1,   2,   1};
    uint8_t pinSufP9[NOCHANNALS] = {  0,   0,   0,   0,   0,   0}; //Check system after system set

    char buff[3];
    char buffer[100];
    uint8_t i;
    PWMstr pinSpecs;

    //Check what header and pin number the IO number has
    for(i = 0; i < NOCHANNALS; i++){    
        if(*io == pinValP8[i]){
            pinSpecs.header = 8;
            pinSpecs.hpin   = pinPosP8[i];
            pinSpecs.pwmPin = pinPwmP8[i];
            pinSpecs.suffix = pinSufP8[i];
            break;
        } else if(*io == pinValP9[i]){
            pinSpecs.header = 9;
            pinSpecs.hpin   = pinPosP9[i];
            pinSpecs.pwmPin = pinPosP9[i];
            pinSpecs.suffix = pinSufP9[i];
            break;
        }
    }

    pinSpecs.io     = *io;
    pinSpecs.period = *period;

    //Check whether all PWM spaces are taken
    //Check whether this pin already is assigned, if so overwrite
    //If new space, add after other spaces
    for(i = 0; i < NOCHANNALS; i++){

        if(channal[i].header != 0){ //Check whether this pin is already assigned
            if(pinSpecs.pwmPin == channal[i].pwmPin){ //If this pin is allready assigned, re-assign
                channal[i] = pinSpecs;
                printf("Changed old entry.\n");
                fflush(stdout);
                break;
            }

            //Can't be true!
            if(i == (NOCHANNALS - 1)){
                printf("Fatal error: PWM pin %i not assigned, slots are full!\n", pinSpecs.pwmPin);
                fflush(stdout);
                return;
            }

        } else {
            channal[i] = pinSpecs;
            printf("Add new entry...\n");
            fflush(stdout);
            break;
        }

    }

    //Assign the period to the output    
    writePWMFile(&channal[i], channal[i].period, PERIOD);
}

/**
 * Writes the assigned value to the assignd filename in the PWMDIR defined by pinInfo.
 *
 * @param pinInfo  = the info required to construct the right file path
 * @param value    = the value to write to the file
 * @param fileName = the filename to write the int value to
 */
void writePWMFile(PWMstr* pinInfo, uint32_t value, char* fileName)
{
    char buff[3];
    char buffer[100];
    int32_t file;

    //Construct adress line
    strcpy(buffer, PWMDIR);
    sprintf(buff, "%i", pinInfo->header);
    strcat(buffer, buff);
    strcat(buffer, "_");
    sprintf(buff, "%i", pinInfo->hpin);
    strcat(buffer, buff);
    strcat(buffer, ".");
    sprintf(buff, "%i", pinInfo->suffix);
    strcat(buffer, buff);
    strcat(buffer, "/");
    strcat(buffer, fileName);

    if((file = open(buffer, O_WRONLY)) != 0){
        sprintf(buffer, "%i", value);

        write(file, buffer, ( sizeof(buffer) / sizeof(char) ));
        close(file);
    } else {
        printf("Faild to open the PWM file. Check path!\n");
        printf("Current file path: %s\n", buffer);
        fflush(stdout);
    }
}

/**
 * Checks whether he assigned pin is a PWM pin.
 * @param io = the io to be assigned as PWM pin
 *
 * @return 1 if this pin can be assigned as PWM pin
 */
uint8_t checkPin(uint8_t* io)
{
    uint8_t ehrpwmPins[12] = {2, 3, 23, 22, 50, 51, 70, 71, 80, 81, 110, 111}; //Calculated IO values
    uint8_t i;

    for(i = 0; i < 12; i++){
        if(*io == ehrpwmPins[i]){
            return 1;
        }
    }

    return 0;
}
