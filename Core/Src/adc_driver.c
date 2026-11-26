/*
 * adc_driver.c
 *
 *  Created on: 25. stu 2026.
 *      Author: tonim
 */

#include "stm32f4xx.h"
#include "adc_driver.h"

static volatile uint16_t adc_value = 0U;
static volatile uint8_t adc_flag = 0U;

void adc1_init(void)
{
	// Enable GPIOA and ADC1 peripheral clocks
	RCC->AHB1ENR |= (1U << 0U);
	RCC->APB2ENR |= (1U << 8U);

	// Configure PA0 in analog mode
	GPIOA->MODER |= (3U << 0U);

	// Disable pull-up and pull-down resistors
	GPIOA->PUPDR &= ~(3U << 0U);

	// Set ADC clock prescaler to /4
	ADC->CCR &= ~(3U << 16U);
	ADC->CCR |= (1U << 16U);

	// Set 12-bit resolution
	ADC1->CR1 &= ~(3U << 24U);

	// Set channel 0 sampling time to 84 cycles
	ADC1->SMPR2 &= ~(7U << 0U);
	ADC1->SMPR2 |= (4U << 0U);

	// Set regular sequence length to 1 conversion
	ADC1->SQR1 &= ~(15U << 20U);

	// Select channel 0 as the first conversion
	ADC1->SQR3 &= ~(31U << 0U);

	// Disable continuous conversion mode
	ADC1->CR2 &= ~(1U << 1U);

	// Set right data alignment
	ADC1->CR2 &= ~(1U << 11U);

	// Disable external trigger
	ADC1->CR2 &= ~(3U << 28U);

	// Enable ADC1
	ADC1->CR2 |= (1U << 0U);

	// Enable ADC interrupt
	ADC1->CR1 |= (1U << 5U);

	NVIC_SetPriority(ADC_IRQn, 3);
	NVIC_EnableIRQ(ADC_IRQn);
}

uint16_t adc1_read(void)
{
	ADC1->CR2 |= (1U << 30U);

    while((ADC1->SR & (1U << 1U)) == 0U)
    {

    }

    return (uint16_t)ADC1->DR;;
}

void adc_irq_handler(void)
{
	if((ADC1->SR & (1U << 1U)) != 0U)
	{
		adc_value = ADC1->DR;
		adc_flag = 1U;
	}
}

void adc1_start_conversion(void)
{
	ADC1->CR2 |= (1U << 30U);
}

uint8_t adc1_read_available(uint16_t *value)
{
	if(adc_flag == 0U)
	{
		return 0U;
	}

	*value = adc_value;

	adc_flag = 0U;

	return 1U;
}
