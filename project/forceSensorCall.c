#include "fsr.h"

#define FSR_SAMPLE_COUNT 16

#define THRESH_H 1445 
#define THRESH_L 989

volatile uint8_t FsrSafetyTriggered = 0;

static uint16_t ADC1_Read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion

    while ((ADC1->SR & ADC_SR_EOC) == 0) // Wait for end of conversion
    {
    }

    return (uint16_t)(ADC1->DR & 0xFFFF); // Return the ADC value
}

void initFsrHW(void)
{
	// ADC1 , chanel 0
	// PA0: Analog Mode (11), No Pull-up (00)
	GPIOA->MODER |= (3U << (0 * 2));
	GPIOA->PUPDR &= ~(3U << (0 * 2));

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	ADC1->SQR3 = 0;              // Sequence 1 = Channel 0
	ADC1->CR2 |= ADC_CR2_ADON;   // Enable ADC
}

/*
void initFsrHW(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC1 clock
    (void)RCC->AHB1ENR;
    (void)RCC->APB2ENR;

    GPIOA->MODER &= ~(3 << 0); // PA0 set as analog mode
    GPIOA->MODER |=  (3 << 0);

    GPIOA->PUPDR &= ~(3 << 0); // No pull up/down on PA0

    ADC1->CR1 = 0; // Clear ADC1 control register 1
    ADC1->CR2 = 0; // Clear ADC1 control register 2

    ADC1->SMPR2 &= ~(7 << 0); // Channel 0 sample time
    ADC1->SMPR2 |=  (7 << 0); // Set highest sample cycles

    ADC1->SQR1 &= ~(0xF << 20); // Only 1 conversion in regular sequence

    ADC1->SQR3 &= ~(0x1F << 0); // SQ1 = channel 0

    ADC1->CR2 &= ~ADC_CR2_CONT; // Single conversion mode
    ADC1->CR2 |= ADC_CR2_ADON; // Turn ADC on

    FsrSafetyTriggered = 0;
}
*/

FsrCondition_t FsrWeightCheck(void)
{
    uint32_t avg = 0;
    uint8_t i;

    ADC1->CR2 &= ~ADC_CR2_CONT; // Make sure ADC is in single conversion mode

    for (i = 0; i < 16; i++)
        avg += ADC1_Read();
    avg >>= 4;

    if (avg < THRESH_L)
        return FSRC_CHICKEN;
    else if (avg > THRESH_H)
        return FSRC_EMPTY;
    else
        return FSRC_EGGONLY;
}

void ConfigureSafetyInterrupt(void)
{
    /*
     * Uses the ADC analog watchdog.
     * The watchdog triggers when the ADC value goes outside the window.
     *
     * We set:
     *   Low threshold = 0
     *   High threshold = 3400
     *
     * Since the ADC cannot go below 0, the interrupt will only happen
     * when the value goes above 3400, meaning a chicken likely returned.
     */

    FsrSafetyTriggered = 0;

    ADC1->SR &= ~ADC_SR_AWD; // Clear old analog watchdog flag

    ADC1->CR1 &= ~0x1F; // Watchdog channel = channel 0
    ADC1->CR1 |= ADC_CR1_AWDSGL; // Watch a single channel
    ADC1->CR1 |= ADC_CR1_AWDEN; // Enable watchdog on regular conversions
    ADC1->CR1 |= ADC_CR1_AWDIE; // Enable watchdog interrupt

    ADC1->LTR = 0; // Lower threshold
    ADC1->HTR = THRESH_H; // Upper threshold

    NVIC_ClearPendingIRQ(ADC_IRQn);
    NVIC_EnableIRQ(ADC_IRQn);

    ADC1->CR2 |= ADC_CR2_CONT; // Continuous conversion mode so ADC keeps sampling
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversions
}

void ADC_IRQHandler(void)
{
    if ((ADC1->SR & ADC_SR_AWD) != 0) // Check if watchdog caused the interrupt
    {
        FsrSafetyTriggered = 1; // Chicken returned to nest

        ADC1->SR &= ~ADC_SR_AWD; // Clear watchdog flag

        ADC1->CR1 &= ~ADC_CR1_AWDIE; // Stop more watchdog interrupts
        ADC1->CR2 &= ~ADC_CR2_CONT; // Stop continuous conversions
    }
}