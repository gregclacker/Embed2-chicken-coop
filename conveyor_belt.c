#include "stm32f4xx.h"
#include "conveyor_belt.h"

// 1 kHz PWM from 16 MHz clock
// f = 16 MHz / (PSC+1) / (ARR+1)
// PSC = 15 → 1 MHz
// ARR = 999 → 1 kHz

#define PWM_PSC  15
#define PWM_ARR  999

void initPWM(void)
{
  // 1) Enable clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // GPIOB
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;    // TIM2

  // 2) PB10 → Alternate Function
  GPIOB->MODER &= ~(3u << (10 * 2));
  GPIOB->MODER |=  (2u << (10 * 2));     // AF mode

  // 3) AF1 → TIM2_CH3
  GPIOB->AFR[1] &= ~(0xFu << ((10 - 8) * 4));
  GPIOB->AFR[1] |=  (1u   << ((10 - 8) * 4));  // AF1

  // 4) Push-pull, high speed
  GPIOB->OTYPER &= ~(1u << 10);
  GPIOB->OSPEEDR |= (3u << (10 * 2));

  // 5) Timer setup
  TIM2->PSC = PWM_PSC;
  TIM2->ARR = PWM_ARR;

  // 50% duty cycle
  TIM2->CCR3 = PWM_ARR / 2;

  // 6) PWM mode 1 on channel 3
  TIM2->CCMR2 &= ~(7u << 4);     // clear OC3M
  TIM2->CCMR2 |=  (6u << 4);     // PWM mode 1
  TIM2->CCMR2 |=  (1u << 3);     // preload enable

  // 7) DO NOT enable output here (leave PWM OFF initially)

  // 8) Enable auto-reload preload
  TIM2->CR1 |= TIM_CR1_ARPE;

  // 9) Start timer (runs in background, no output yet)
  TIM2->CR1 |= TIM_CR1_CEN;
}

void PWM_on(void)
{
  // Enable PWM output on PB10
  TIM2->CCER |= TIM_CCER_CC3E;
}

void PWM_off(void)
{
  // Disable PWM output (pin goes idle)
  TIM2->CCER &= ~TIM_CCER_CC3E;
}