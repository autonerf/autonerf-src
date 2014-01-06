/**
 * This file controls the missiles
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpio.h>
#include <launcher.h>

/* Global variables */
bool present[10] = {false, false, false, false, false,
                    false, false, false, false, false};
uint8_t chamber[10];
uint8_t disablePulse;
uint8_t feedback;
uint8_t reloadLED;
uint8_t reloadSw;

/* Prototypes */
bool isPresent(void);
int8_t shootNext(void);

void initLauncher(void)
{
    uint8_t i;

    chamber[0]   = calculateGPIO(2, 14); //Pull-down enabled
    chamber[1]   = calculateGPIO(2, 12); //Pull-down enabled
    chamber[2]   = calculateGPIO(2, 10); //Pull-down enabled
    chamber[3]   = calculateGPIO(2, 8);  //Pull-down enabled
    chamber[4]   = calculateGPIO(2, 6);  //Pull-down enabled
    chamber[5]   = calculateGPIO(2, 7);  //Pull-down enabled
    chamber[6]   = calculateGPIO(2, 9);  //Pull-down enabled
    chamber[7]   = calculateGPIO(2, 11); //Pull-down enabled
    chamber[8]   = calculateGPIO(2, 13); //Pull-down enabled
    chamber[9]   = calculateGPIO(2, 15); //Pull-down enabled
    reloadLED    = calculateGPIO(2, 5);  //Floating
    disablePulse = calculateGPIO(0, 9);  //Pull-up enabled
    reloadSw     = calculateGPIO(0, 2);  //Pull-up enabled
    feedback     = calculateGPIO(0, 8);  //Floating

    for(i = 0; i < 10; i++){
        initGPIO(&chamber[i], OUTPUT_PIN); //Pull-down enabled
    }
    initGPIO(&reloadLED, OUTPUT_PIN);       //Floating
    initGPIO(&disablePulse, OUTPUT_PIN);    //Pull-up enabled
    initGPIO(&reloadSw, INPUT_PIN);         //Pull-up enabled
    initGPIO(&feedback, INPUT_PIN);         //Floating

    for(i = 0; i < 10; i++){
        setOutput(&chamber[i], '0');
    }
    setOutput(&reloadLED, '0');
    setOutput(&disablePulse, '1');
}

/**
 * Launches a missile if present
 */
int8_t shoot(void)
{
    int8_t chamerShot = -1;

    if(isPresent()){
        chamerShot = shootNext();
    }

    return chamerShot;
}


/**
 * Enables the system so it can shoot again.
 */
void reload(void)
{
    uint8_t i;

    if(getInput(&reloadSw) == 0){
        setOutput(&reloadLED, '0');

        for(i = 0; i < 10; i++){
            present[i] = true;
        }
    }
}

/**
 * Unexports all GPIO pins before quiting the program.
 */
void quitLauncher(void)
{
    uint8_t i;

    for(i = 0; i < 10; i++){
        unexportGPIO(&chamber[i]);
    }
}

/*******************************************************************************
 ******************************* LOCAL FUNCTIONS *******************************
 ******************************************************************************/

/**
 * Checks whether a missile is present.
 * @return true if there is a missile present
 */
bool isPresent(void)
{
    uint8_t i;

    for(i = 0; i < 10; i++){
        if(present[i]){
            return true;
        }
    }

    return false;
}

int8_t shootNext(void)
{
    uint8_t i;

    for(i = 0; i < 10; i++){
        if(present[i]){
            break;
        }
    }

    present[i] = false;

    setOutput(&chamber[i], '1');
    usleep(100000);
    setOutput(&chamber[i], '0');

    if(i >= 9){
        setOutput(&reloadLED, '1');

        while(getInput(&feedback) == 1){
            setOutput(&disablePulse, '0');
            usleep(10);
            setOutput(&disablePulse, '1');
            usleep(10);
        }
    }

    return (int8_t)(i);
}
