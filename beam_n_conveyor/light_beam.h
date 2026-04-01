#ifndef LIGHT_BEAM_H_
#define LIGHT_BEAM_H_

#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initSPI1();
void cs_low();
void cs_high();
void spi1_clear_ovr();
void write_reg(uint8_t reg, uint8_t val);
uint8_t read_reg(uint8_t reg);
void initMCP();
uint8_t spi1_txrx(uint8_t data);
void beam_toggle(bool light); // Turn on/off transmitter
bool beam_check(void); // Check if Beam is broken (returns false if it is)
bool beam_integrity(); // Runs full checking procedure by turning on LED and checking beam integrity for 5 secs

#endif