/******************************************************************************
 * pwm.h                                                                      *
 *                                                                            *
 * Liberary to access and control the PWM pins.                               *
 *****************************************************************************/

#ifndef PWM
#define PWM

/* Definitions */
#define NOCHANNALS 6

/* Prototypes */
void initPWM(uint8_t* io, uint32_t period);
void setDutyCycle(uint8_t* io, uint32_t newValue);
uint32_t freqToPeriod(uint32_t frequency);
uint8_t calculatePWM(uint8_t chip, uint8_t pin);

#endif /* PWM */
