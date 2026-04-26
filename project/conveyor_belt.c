#include "stm32f4xx.h"
#include "conveyor_belt.h"

// 1 kHz PWM from 16 MHz clock
// f = 16 MHz / (PSC+1) / (ARR+1)
// choose:
// PSC = 15  ? 1 MHz
// ARR = 999 ? 1 kHz

#define PWM_PSC  15
#define PWM_ARR  999

void initPWM(void)
{
  // 1) Enable clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // GPIOBs
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;    // TIM1

  // 2) PB13 ? Alternate Function
  GPIOB->MODER &= ~(3u << (13 * 2));
  GPIOB->MODER |=  (1u << (13 * 2));   // Output mode
}

void PWM_on(void)
{
  // Enable output
  GPIOB->BSRR |= 0x2000;
}

void PWM_off(void)
{
  // Disable output (forces pin idle)
  GPIOB->BSRR |= 0x2000 << 16;
}