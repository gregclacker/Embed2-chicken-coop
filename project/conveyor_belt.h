#ifndef CONVEYOR_BELT_H
#define CONVEYOR_BELT_H

#include <stdint.h>

// Initialize PWM hardware (timer, GPIO, etc.)
void initPWM(void);

// Enable PWM output
void PWM_on(void);

// Disable PWM output
void PWM_off(void);

#endif // CONVEYOR_BELT_H