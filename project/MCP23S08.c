#include "MCP23S08.h"

void MCP23S08_write(const MCP23S08_t * mcp,MCP23S08_REG_t reg, uint8_t data) {
	if(!mcp || !mcp->spi)
		return;
	
	uint8_t tx[] = {
			0x40 | (mcp->addr_ext << 1),
			reg,
			data
	};
	
	SPI_transfer8(mcp->spi, tx, 0, 3, &mcp->cs);
}

uint8_t MCP23S08_read(const MCP23S08_t * mcp, MCP23S08_REG_t reg) {
	if(!mcp || !mcp->spi)
		return 0;
	
	uint8_t tx[] = {
			0x40 | (mcp->addr_ext << 1) | 1,
			reg,
			0
	};
	
	SPI_transfer8(mcp->spi, tx, tx, 3, &mcp->cs);
	
	return tx[2];
}

void initMCPSPI() {
	/*
		PB4		SPI1 	MISO
		PB5		SPI1 	MOSI
		PB3		SPI1 	SCK
		PA2		SPI1 	CS
	*/
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // SPI1 clk
	
	// PA10 : DO : CS
	/*
	_Static_assert(MCPCS.port == GPIOA && MCPCS.pinN == 10,"bruh");
	MCPCS.port->MODER &= ~(0b11 << (2 * MCPCS.pinN));
	MCPCS.port->MODER |= 	(0b01 << (2 * MCPCS.pinN)); // DO
	GPIO_setOut(&MCPCS, !MCPCS_ON);
	*/
	
	// PB4 : SPI1 MISO
	GPIOB->MODER &= ~(0b11 << (4*2));	// set to alternative mode
	GPIOB->MODER |= (0b10 << 	(4*2));
	GPIOB->AFR[0] &= ~(0xF << (4*4)); // set alf to 5:MISO
	GPIOB->AFR[0] |= (0x5 << 	(4*4));
	
	// PB5 : SPI1 MOSI
	GPIOB->MODER &= ~(0b11 << (5*2));	// set to alternative mode
	GPIOB->MODER |= (0b10 << 	(5*2));
	GPIOB->AFR[0] &= ~(0xF << (5*4)); // set alf to 5:MOSI
	GPIOB->AFR[0] |= (0x5 << 	(5*4));
	
	// PB3 : SPI1 CLK
	GPIOB->MODER &= ~(0b11 << (3*2));
	GPIOB->MODER |= (0b10 << 	(3*2));
	GPIOB->AFR[0] &= ~(0xF << (3*4)); // set alf to 5:CLK
	GPIOB->AFR[0] |= (0x5 << 	(3*4));
	
	SPI1->CR1 &= ~(1 << 6); // disable spi
	SPI1->CR1 &= ~(1 << 7); // MSB first
	SPI1->CR1 &= ~(1 << 11); // 8 bit data
	SPI1->CR1 &= ~(0b111 << 3); // set baud
	SPI1->CR1 |= 	(4 << 3); // set baud
	SPI1->CR1 &= ~(1 << 13); // disable CRC
	SPI1->CR1 &= ~(1 << 15); // normal 2 line spi 
	SPI1->CR1 &= ~(1 << 10); // trx 
	SPI1->CR1 |= (0b11 << 8); // software slave management
	SPI1->CR1 |= 	(1 << 2); // SPI master
	SPI1->CR1 &= ~(1 << 1); // SPI POL
	SPI1->CR1	&= ~(1 << 0); // SPI PHA
	SPI1->CR1 |= 	(1 << 6); // enable spi
}
