/*
 * pwm_driver.c
 *
 *  Created on: 27. stu 2026.
 *      Author: tonim
 */

#include "stm32f4xx.h"
#include "pwm_driver.h"

void pwm_init(void)
{
	// Enable GPIOA and TIM3 peripheral clocks
	RCC->AHB1ENR |= (1U << 0U);
	RCC->APB1ENR |= (1U << 1U);

	// Configure PA6 in alternate function mode
	GPIOA->MODER &= ~(3U << 12U);
	GPIOA->MODER |= (2U << 12U);

	// Select AF2 (TIM3_CH1)
	GPIOA->AFR[0] &= ~(15U << 24U);
	GPIOA->AFR[0] |= (2U << 24U);

	// Configure PWM frequency to 1 kHz
	TIM3->PSC = 83U;
	TIM3->ARR = 999U;

	// Configure channel 1 as output
	TIM3->CCMR1 &= ~(3U << 0U);

	// Configure PWM mode 1
	TIM3->CCMR1 &= ~(7U << 4U);
	TIM3->CCMR1 |= (6U << 4U);

	// Enable output compare preload
	TIM3->CCMR1 |= (1U << 3U);

	// Set output polarity to active HIGH
	TIM3->CCER &= ~(1U << 1U);

	// Set initial duty cycle to 50%
	TIM3->CCR1 = 50U;

	// Generate update event
	TIM3->EGR = (1U << 0U);

	// Enable channel 1 output
	TIM3->CCER |= (1U << 0U);

	// Start TIM3 counter
	TIM3->CR1 |= (1U << 0U);
}

void pwm_set_duty(uint8_t duty_percent)
{
	if(duty_percent > 100U)
	{
		duty_percent = 100U;
	}

	TIM3->CCR1 = (duty_percent * (TIM3->ARR + 1U)) / 100U;
}
