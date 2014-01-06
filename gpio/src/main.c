#include <stdio.h>
#include <stdint.h>
#include "../header/launcher.h"
#include "../header/pantilt.h"
#include "../header/gpio.h"

#define WORDLENGTH 100

/* Local prototype */
void panControl(char* input);
void tiltControl(char* input);

/**
 * Main
 */
int main()
{    
    uint8_t runLED = calculateGPIO(2, 2);
    char input[WORDLENGTH];
    int8_t temp;

    initLauncher();
    initPanTilt();
    initGPIO(&runLED, OUTPUT_PIN);

    printf("Init complete, run!\n");
    fflush(stdout);

    setOutput(&runLED, '1');

    while(1){
        fgets(input, WORDLENGTH, stdin);
        fflush(stdin);

        if(strcmp(input, "pan\n") == 0){
            panControl(input);
        } else if(strcmp(input, "tilt\n") == 0){
            tiltControl(input);
        } else if(strcmp(input, "launch\n") == 0){
            temp = shoot();
            if(temp == -1){
                printf("No missiles left!\n");
                fflush(stdout);
            } else {
                printf("Chamber shot: %i\n", (temp + 1));
                fflush(stdout);
            }
        } else if(strcmp(input, "reload\n") == 0){
            reload();
        } else if(strcmp(input, "quit\n") == 0){
            break;
        } else {
            printf("Invalid conmmand.\n");
            fflush(stdout);
        }
    }

    setOutput(&runLED, '0');

    quitLauncher();
    
    return 0;
}

void panControl(char* input)
{
    uint8_t stringCount;
    uint8_t msd; //Moste significant digit
    uint8_t md;  //Middel digit
    uint8_t lsd; //Least significant digit
    float value; //Numeric value of input string

    printf("Enter the pan angle in degrees.\n");

    while(1){
        fgets(input, WORDLENGTH, stdin);
        fflush(stdin);

        if(strcmp(input, "back\n") == 0){
            return;
        }

        stringCount = sizeof(input) / sizeof(char);

        if(stringCount == 4){
            msd = input[0] - 48;
            md  = input[1] - 48;
            lsd = input[2] - 48;

            value = (float)((msd * 100) + (md * 10) + (lsd));

            setPan(value);
        } else {
            printf("The number has to contain 3 digets!\n");
            fflush(stdout);
        }
    }
}

void tiltControl(char* input)
{
    uint8_t stringCount;
    uint8_t msd; //Moste significant digit
    uint8_t md;  //Middel digit
    uint8_t lsd; //Least significant digit
    float value; //Numeric value of input string

    printf("Enter the tilt angle in degrees.\n");

    while(1){
        fgets(input, WORDLENGTH, stdin);
        fflush(stdin);

        if(strcmp(input, "back\n") == 0){
            return;
        }

        stringCount = sizeof(input) / sizeof(char);

        if(stringCount == 4){
            msd = input[0] - 48;
            md  = input[1] - 48;
            lsd = input[2] - 48;

            value = (float)((msd * 100) + (md * 10) + (lsd));

            setTilt(value);
        } else {
            printf("The number has to contain 3 digets!\n");
            fflush(stdout);
        }
    }
}
