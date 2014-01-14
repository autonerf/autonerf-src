#ifndef AUTONERF_IO_PWM_H
#define AUTONERF_IO_PWM_H

// Standard C dependencies
#include <stdint.h>

// Defines
#define PWM_DIRECTORY       "/sys/devices/ocp.2/pwm_test_P"
#define PWM_FILE_DUTY       "duty"
#define PWM_FILE_PERIOD     "period"
#define PWM_CHANNEL_COUNT   6

typedef struct {
    uint8_t     io;
    uint8_t     header;
    uint8_t     hpin;
    uint8_t     pwm_pin;
    uint16_t    suffix;
    uint32_t    period;
} pwm_t;

/**
 Initialize the PWM period `period` on the pin `io`

 @param io     The I/O pin to use
 @param period The period of the PWM signal in nanoseconds
 */
extern void pwm_init(register const uint8_t io, register const uint32_t period);

/**
 Set the frequency of a PWM I/O pin

 @param io   The I/O pin to use
 @param freq The frequency to use
 */
extern void pwm_set_frequency(register const uint8_t io, register const uint32_t freq);

/**
 Set the duty cycle of a PWM I/O pin

 @param io    The I/O pin to use
 @param cycle The duty cycle to use
 */
extern void pwm_set_duty_cycle(register const uint8_t io, register const uint32_t cycle);

#endif /* AUTONERF_IO_PWM_H */
