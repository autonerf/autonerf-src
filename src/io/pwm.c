// Standard C dependencies
#include <stdio.h>

// Project dependencies
#include <autonerf/logger.h>
#include <autonerf/io/pwm.h>
#include <autonerf/io/gpio.h>

pwm_t _channels[PWM_CHANNEL_COUNT] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

/**
 Write the assigned value to the assigned filename in the PWM_DIRECTORY defined by `info`

 @param info  The info required to construct the correct file path
 @param value The value to write the the configuration value
 @param file  The filename to write the integer value to
 */
void
pwm_write_file(pwm_t * info, register const uint32_t value, const char * file)
{
    FILE *  fp;
    char    filename[1024];

    sprintf(filename, "%s%d_%d.%d/%s", PWM_DIRECTORY, info->header, info->hpin, info->suffix, file);

    if ((fp = fopen(filename, "w")) != NULL) {
        fprintf(fp, "%d", value);
        fclose(fp);
    } else {
        LOG_ERROR(
            "Failed to open the PWM file, check path: %s",
            filename
        );
    }
}

/**
 Check if the assigned I/O pin is a PWM pin.

 @param io The I/O to be checked
 @return Returns 1 if this pin can be used as PWM pin
 */
uint8_t
pwm_check_pin(register const uint8_t io)
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
 Convert a frequency to a period

 @param frequency The frequency in [Hz]
 @return The period in nanoseconds
 */
uint32_t
pwm_freq_to_period(register const uint32_t frequency)
{
    return (1000000000 / frequency);
}

void
pwm_init(register const uint8_t io, register const uint32_t period)
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

void
pwm_set_duty_cycle(register const uint8_t io, register const uint32_t cycle)
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

    pwm_write_file(&_channels[i], cycle, PWM_FILE_DUTY);
}

void
pwm_set_frequency(register const uint8_t io, register const uint32_t period)
{
    uint8_t pin_identifier[2][PWM_CHANNEL_COUNT] = {
        {23, 22, 81, 80, 70, 71},
        {50, 51, 3, 2, 111, 110}
    };

    uint8_t pin_position[2][PWM_CHANNEL_COUNT] = {
        {12, 19, 34, 36, 45, 46},
        {14, 16, 21, 22, 29, 31}
    };

    uint8_t pin_generator[2][PWM_CHANNEL_COUNT] = {
        {6, 5, 4, 3, 5, 6},
        {3, 4, 2, 1, 2, 1}
    };

    uint8_t pin_suffix[2][PWM_CHANNEL_COUNT] = {
        {0, 10, 0, 11, 0, 0},
        {0, 0, 0, 0, 0, 0}
    };

    uint8_t i;
    pwm_t   info;

    for (i = 0; i < PWM_CHANNEL_COUNT; i++) {
        register uint8_t header = 2;

        if (io == pin_identifier[0][i]) { header = 0; }
        if (io == pin_identifier[1][i]) { header = 1; }

        if (header != 2) {
            info.header  = (header + 8);
            info.hpin    = pin_position[header][i];
            info.pwm_pin = pin_generator[header][i];
            info.suffix  = pin_suffix[header][i];
        }
    }

    info.io     = io;
    info.period = period;

    for (i = 0; i < PWM_CHANNEL_COUNT; i++) {
        if (_channels[i].header != 0) {
            if (info.pwm_pin == _channels[i].pwm_pin) {
                _channels[i] = info;
                LOG_INFO("Changed PWM channel entry %d", i);

                break;
            }
        } else {
            _channels[i] = info;
            LOG_INFO("Assigned new PWM channel to entry %d", i);

            break;
        }
    }

    if (i == PWM_CHANNEL_COUNT) {
        LOG_EMERGENCY("Couldn't assign PWM I/O pin %d, all slots are full", io);

        return;
    }

    pwm_write_file(&_channels[i], _channels[i].period, PWM_FILE_PERIOD);
}
