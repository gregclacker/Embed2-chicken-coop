#include <stdint.h>
#include "common.h"

#define MCPCS_ON 0

typedef enum _MCP23S08_REG : uint8_t {
	REG_IODIR,
	REG_IPOL,
	REG_GPINTEN,
	REG_DEFVAL,
	REG_INTCON,
	REG_IOCON,
	REG_GPPU,
	REG_INTF,
	REG_INTCAP,
	REG_GPIO,
	REG_OLAT,
} MCP23S08_REG_t;
_Static_assert(REG_OLAT == 0xA, "messed up register addresses");

typedef struct _MCP23S08 {
	GPIO_Pin_t cs;
	SPI_TypeDef * spi;
	
	uint8_t addr_ext : 2;
} MCP23S08_t;

void initMCPSPI(); // project specific. hardcoded pins and what not
void MCP23S08_write(const MCP23S08_t *,MCP23S08_REG_t reg, uint8_t data);
uint8_t MCP23S08_read(const MCP23S08_t *, MCP23S08_REG_t reg);
