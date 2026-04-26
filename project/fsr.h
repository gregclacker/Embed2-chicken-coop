#ifndef FSR_H
#define FSR_H

#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

typedef enum FsrCondition_e {
	FSRC_EMPTY = 1,
	FSRC_EGGONLY = 2,
	FSRC_CHICKEN = 3,
} FsrCondition_t;

extern const GPIO_Pin_t
	ForceInput;

extern volatile uint8_t FsrSafetyTriggered;

void initFsrHW(void);
FsrCondition_t FsrWeightCheck(void);
void ConfigureSafetyInterrupt(void);

#endif