#ifndef AUTONERF_IO_PWM_H
#define AUTONERF_IO_PWM_H

// Standard C dependencies
#include <stdint.h>

// Defines
#define PWM_DIRECTORY       "/sys/devices/ocp.2/pwm_test_P"
#define PWM_FILE_DUTY       "duty"
#define PWM_FILE_PERIOD     "period"
#define PWM_CHANNEL_COUNT   6

struct {
    uint8_t     io;
    uint8_t     header;
    uint8_t     hpin;
    uint8_t     pwm_pin;
    uint16_t    suffix;
    uint32_t    period;
} pwm_t;

extern void pwm_init(const register uint8_t io, const register uint32_t period);
extern void pwm_set_frequency(const register uint8_t io, const register uint32_t cycle);
extern void pwm_set_duty_cycle(const register uint8_t io, const register uint32_t cycle);

void initPWM(uint8_t* io, uint32_t period);
void setDutyCycle(uint8_t* io, uint32_t newValue);
uint32_t freqToPeriod(uint32_t frequency);
uint8_t calculatePWM(uint8_t chip, uint8_t pin);

#endif /* AUTONERF_IO_PWM_H */
