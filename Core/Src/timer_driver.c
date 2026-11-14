/*
 * timer_driver.c
 *
 *  Created on: 14. stu 2026.
 *      Author: tonim
 */

#include "stm32f4xx.h"
#include "timer_driver.h"
#include "gpio_driver.h"

static volatile uint32_t tim2_pending_ticks = 0U;

void tim2_init(void)
{
    // Enable TIM2 peripheral clock
    RCC->APB1ENR |= (1U << 0U);

    // 2. Configure 500 ms period
	TIM2->PSC = 8399U;
	TIM2->ARR = 4999U;

	// 3. Load prescaler
	TIM2->EGR = (1U << 0U);

	// 4. Clear update flag
	TIM2->SR &= ~(1U << 0U);

	// 5. Enable update interrupt inside TIM2
	TIM2->DIER |= (1U << 0U);

	// 6. Enable TIM2 interrupt in NVIC
	NVIC_EnableIRQ(TIM2_IRQn);

	// 7. Start timer
	TIM2->CR1 |= (1U << 0U);
}

void tim2_irq_handler(void)
{
    if ((TIM2->SR & (1U << 0U)) != 0U)
    {
        TIM2->SR &= ~(1U << 0U);

        tim2_pending_ticks++;
    }
}

uint8_t tim2_take_tick(void)
{
    NVIC_DisableIRQ(TIM2_IRQn);

    if (tim2_pending_ticks == 0U)
    {
        NVIC_EnableIRQ(TIM2_IRQn);
        return 0U;
    }

    tim2_pending_ticks--;

    NVIC_EnableIRQ(TIM2_IRQn);
    return 1U;
}
