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
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // GPIOAs
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;    // TIM1

  // 2) PB13 ? Alternate Function
  GPIOB->MODER &= ~(3u << (13 * 2));
  GPIOB->MODER |=  (2u << (13 * 2));   // AF mode

  // 3) AF1 for TIM1
  GPIOB->AFR[1] &= ~(0xFu << ((13 - 8) * 4));
  GPIOB->AFR[1] |=  (1u   << ((13 - 8) * 4));  // AF1

  // 4) Push-pull, high speed
  GPIOB->OTYPER &= ~(1u << 13);
  GPIOB->OSPEEDR |= (3u << (13 * 2));

  // 5) Timer setup
  TIM1->PSC = PWM_PSC;
  TIM1->ARR = PWM_ARR;

  // 50% duty cycle
  TIM1->CCR1 = PWM_ARR / 2;

  // 6) PWM mode 1 on channel 1
  TIM1->CCMR1 &= ~(7u << 4);
  TIM1->CCMR1 |=  (6u << 4);   // PWM mode 1
  TIM1->CCMR1 |=  (1u << 3);   // preload enable

  // 7) Enable complementary output (CH1N)
  TIM1->CCER |= TIM_CCER_CC1NE;

  // 8) Enable main output (REQUIRED for TIM1)
  TIM1->BDTR |= TIM_BDTR_MOE;

  // 9) Enable auto-reload preload
  TIM1->CR1 |= TIM_CR1_ARPE;

  // 10) Start timer
  TIM1->CR1 |= TIM_CR1_CEN;
}

void PWM_on(void)
{
  // Enable output
  TIM1->CCER |= TIM_CCER_CC1NE;
}

void PWM_off(void)
{
  // Disable output (forces pin idle)
  TIM1->CCER &= ~TIM_CCER_CC1NE;
}