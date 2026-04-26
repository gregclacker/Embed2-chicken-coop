#include <stdbool.h>
#include "stm32f4xx.h"
#include <stdint.h>
#include "common.h"
#include "light_beam.h"

#define IODIR    0x00
#define GPINTEN  0x02
#define DEFVAL   0x03
#define INTCON   0x04
#define IOCON    0x05
#define GPPU     0x06
#define INTCAP   0x08
#define GPIO     0x09
#define OLAT     0x0A

// Initialize SPI1
void initSPI1()
{
  RCC->AHB1ENR |= 2; // Enable GPIOB clock
  RCC->APB2ENR |= 0x1000; // Enable SPI1 Clock
  
  // SPI1 SCK: PB3 alt function
  // SPI1 MOSI: PB5 alt function
  // SPI1 MISO: PB4 alt function
  GPIOB->MODER &= ~0xFC0; // Clear pin modes
  GPIOB->MODER |= 0xA80; // Set Alternate function mode for SPI1
  GPIOB->AFR[0] &= ~0xFFF000; // Clear Alternate functions for PB3,PB4,PB5
  GPIOB->AFR[0] |= 0x555000; // AF5 for PB3,PB4,PB5 which are on AFRL
  
  // SPI1: Master, Mode 0 (CPOL=0, CPHA=0), 
  // 8-bit, SSM=1, SSI=1
  
  SPI1->CR1 = 0;  // reset everything
  SPI1->CR1 |= (1 << 2);      // Master
  SPI1->CR1 |= (1 << 8);      // SSI
  SPI1->CR1 |= (1 << 9);      // SSM
  SPI1->CR1 |= (0x3 << 3);    // Prescaler /16 (faster than /128)
  
  SPI1->CR1 |= (1 << 6);      // Enable SPI1
	
	// CS
	RCC->AHB1ENR |= 1;        // GPIOA clock
  GPIOA->MODER &= ~0x300000;     // Clear PA10 Mode
  GPIOA->MODER |=  0x100000;     // Set PA10 output Mode
  GPIOA->ODR |= 0x400;        // CS HIGH (idle)
	
	RCC->AHB1ENR |= 3; // Enable Port C
	GPIOC->MODER &= ~	(0b11 << (2 * 3));
}

void cs_low()
{
  GPIOA->ODR &= ~0x400;
	delaymS(10);
}

void cs_high()
{
  GPIOA->ODR |= 0x400;
}

uint8_t spi1_txrx(uint8_t data)
{
  while (!(SPI1->SR & (1 << 1))); // TXE

  *((volatile uint8_t*)&SPI1->DR) = data;

  while (SPI1->SR & (1 << 7));    // BSY

  while (!(SPI1->SR & (1 << 0))); // RXNE

  return *((volatile uint8_t*)&SPI1->DR);
}

void spi1_clear_ovr()
{
  volatile uint8_t temp;

  temp = SPI1->DR;
  temp = SPI1->SR;

  (void)temp;
}

void write_reg(uint8_t reg, uint8_t val)
{
  cs_low();

  spi1_txrx(0x40); // write opcode
  spi1_txrx(reg);
  spi1_txrx(val);

  while (SPI1->SR & (1 << 7)); // BSY

  cs_high();
}

uint8_t read_reg(uint8_t reg)
{
  uint8_t val;

  cs_low();

  spi1_txrx(0x41); // read opcode
  spi1_txrx(reg);
  val = spi1_txrx(0x00); // dummy

  while (SPI1->SR & (1 << 7)); // BSY

  cs_high();

  return val;
}

void initMCP()
{
  // 1. Enable hardware addressing (HAEN bit = bit 3)
  write_reg(IOCON, 0x08);

  // 2. Set pin directions
  // GP1 = output (IR transmitter)
  // GP4 = input  (IR receiver)
  write_reg(IODIR, (1 << 4));

  // 3. Initialize outputs
  // GP1 off initially
  write_reg(OLAT, 0x00);
}

void beam_toggle(bool light)
{
  if (light)
    write_reg(OLAT, (1 << 1));
  else
    write_reg(OLAT, 0x00);
}

bool beam_check(void)
{
  return (GPIOC->IDR & 0x8);
}

bool beam_integrity(void)
{
  beam_toggle(true);

  delaymS(100);

  for (int i = 0; i < 5; i++)
  {
    if (!beam_check())
    {
      beam_toggle(false);
      return false;
    }

    delaymS(1000);
  }

  beam_toggle(false);
  return true;
}

