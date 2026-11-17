/*
 * exti_driver.c
 *
 *  Created on: 17. stu 2026.
 *      Author: tonim
 */

#include "stm32f4xx.h"
#include "exti_driver.h"
#include "systick_driver.h"

#define BUTTON_DEBOUNCE_MS 30U

static volatile uint32_t button_press_count = 0U;
static uint32_t last_press_time = 0U;
static uint8_t first_press = 1U;

void exti13_init(void)
{
    // Enable GPIOC clock
    RCC->AHB1ENR |= (1U << 2U);

    // Configure PC13 as input (MODER13 = 00)
    GPIOC->MODER &= ~(3U << 26U);

    // Enable SYSCFG clock
    RCC->APB2ENR |= (1U << 14U);

    // Select GPIOC as source for EXTI13
    SYSCFG->EXTICR[3] &= ~(0xFU << 4U);
    SYSCFG->EXTICR[3] |=  (0x2U << 4U);

    // Enable rising edge trigger for EXTI13
    EXTI->RTSR |= (1U << 13U);

    // Disable falling edge trigger for EXTI13
    EXTI->FTSR &= ~(1U << 13U);

    // Unmask EXTI13 interrupt
    EXTI->IMR |= (1U << 13U);

    // Clear any pending EXTI13 interrupt
    EXTI->PR = (1U << 13U);

    // Configure NVIC priority
    NVIC_SetPriority(EXTI15_10_IRQn, 3U);

    // Enable EXTI interrupt in NVIC
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void exti13_irq_handler(void)
{
    if ((EXTI->PR & (1U << 13U)) != 0U)
    {
        // Clear pending flag
        EXTI->PR = (1U << 13U);

        uint32_t now = systick_get_ms();

        if ((first_press != 0U) ||
            ((uint32_t)(now - last_press_time) >= BUTTON_DEBOUNCE_MS))
        {
            button_press_count++;
            last_press_time = now;
            first_press = 0U;
        }
    }
}

uint8_t exti13_take_press(void)
{
    uint32_t irq_enabled = NVIC_GetEnableIRQ(EXTI15_10_IRQn);

    NVIC_DisableIRQ(EXTI15_10_IRQn);

    uint8_t press_available = 0U;

    if (button_press_count > 0U)
    {
        button_press_count--;
        press_available = 1U;
    }

    if (irq_enabled != 0U)
    {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }

    return press_available;
}
