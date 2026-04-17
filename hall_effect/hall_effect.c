#include <stdbool.h>
#include "stm32f4xx.h"
#include "hall_effect.h"

// Uses PA6 for Far Side, PA7 for Near Side
void initHallEffect()
{
	RCC->AHB1ENR |= 0x1; // Enable GPIOA clock
	GPIOA->MODER &= ~0xF000; // Clear PA6 and PA7 mode
}

bool getFarSensorStatus()
{
	return (GPIOA->IDR & 0x40);
}

bool getNearSensorStatus()
{
	return (GPIOA->IDR & 0x80);
}