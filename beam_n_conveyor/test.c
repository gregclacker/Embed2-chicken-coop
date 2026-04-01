#include "stm32f4xx.h"
#include "conveyor_belt.h"
#include "light_beam.h"

int main(void)
{
  initPWM();

  PWM_on();
  PWM_off();
  PWM_on();

  while (1)
  {
  }
}