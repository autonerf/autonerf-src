// Standard C dependencies
#include <stdio.h>

// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/pwm.h>
#include <autonerf/io/gpio.h>

struct pwm_t _channels[PWM_CHANNEL_COUNT] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

/**
 * Writes the assigned value to the assignd filename in the PWMDIR defined by pinInfo.
 *
 * @param pinInfo  = the info required to construct the right file path
 * @param value    = the value to write to the file
 * @param fileName = the filename to write the int value to
 */
void
pwm_write_file(struct pwm_t * info, const register uint32_t value, const char * file)
{
    FILE *  file;
    char    filename[1024];

    sprintf(filename, "%s%d_%d.%d/%s" PWM_DIRECTORY, info->header, info->hpin, info->suffix, filename);

    if ((file = fopen(filename, "w")) != NULL) {
        fprintf(file, "%d", value);
        fclose(file);
    } else {
        LOG_ERROR(
            "Failed to open the PWM file, check path: %s",
            filename
        );
    }
}

/**
 * Checks whether he assigned pin is a PWM pin.
 * @param io = the io to be assigned as PWM pin
 *
 * @return 1 if this pin can be assigned as PWM pin
 */
uint8_t
pwm_check_pin(const register uint8_t io)
{
    static uint8_t  pins[] = {2, 3, 23, 22, 50, 51, 70, 71, 80, 81, 110, 111};
    uint8_t         i      = 0;

    for(; i < (sizeof(pins) / sizeof(uint8_t)); i++){
        if(io == pins[i]){
            return 1;
        }
    }

    return 0;
}

/**
 * Simple tool to convert the frequency the required period time.
 * @param frequency = frequency in Hz
 *
 * @return the period time in nano seconds
 */
uint32_t
pwm_freq_to_period(const register uint32_t frequency)
{
    return (1000000000 / frequency);
}

/**
 * Initializes the PWM period cycle on the pin.
 *
 * @param chip   = the chip the pin is on
 * @param pin    = the pin of the chip
 * @param period = the period of the PWM signal in nano seconds
 */
void
pwm_init(const register uint8_t io, const register uint32_t period)
{
    if (!pwm_check_pin(io)) {
        LOG_ERROR(
            "Pin %d:%d is not a PWM pin",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }

    pwm_set_frequency(io, period);
    pwm_set_duty_cycle(io, 0);
}

/**
 * Writes a value to a PWM pin.
 * Interface to uint8_t instead of PWMstr*!
 */
void
pwm_set_duty_cycle(const register uint8_t io, const register uint32_t cycle)
{
    register uint8_t i = 0;

    for (; i < PWM_CHANNEL_COUNT; i++) {
        if (_channels[i].io == io) {
            break;
        }
    }

    if (i >= PWM_CHANNEL_COUNT) {
        LOG_ERROR(
            "GPIO[%d]%d is not a PWM pin or is not initialized",
            (io / GPIO_PIN_COUNT),
            (io % GPIO_PIN_COUNT)
        );
    }

    pwm_write_file(&_channels[i], value, PWM_FILE_DUTY);
}

void
pwm_set_frequency(const register uint8_t io, const register uint32_t period)
{
    LOG_EMERGENCY("Function not implemented yet: %s", __PRETTY_FUNCTION__);
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
