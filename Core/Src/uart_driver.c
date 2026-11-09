/*
 * uart_driver.c
 *
 *  Created on: 9. stu 2026.
 *      Author: tonim
 */

#include "uart_driver.h"
#include "stm32f446xx.h"

void uart2_init(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= (1U << 0U);
    (void)RCC->AHB1ENR;

    // PA2: Alternate Function mode
    GPIOA->MODER &= ~(3U << (2U * 2U));
    GPIOA->MODER |=  (2U << (2U * 2U));

    // PA2: AF7 = USART2_TX
    GPIOA->AFR[0] &= ~(0xFU << (4U * 2U));
    GPIOA->AFR[0] |=  (7U << (4U * 2U));

    // Enable USART2 clock
    RCC->APB1ENR |= (1U << 17U);
    (void)RCC->APB1ENR;

    // Baud rate: PCLK1 = 42 MHz, baud = 115200
    // Oversampling by 16
    USART2->BRR = 365U;

    // Enable transmitter and USART
    USART2->CR1 |= (1U << 3U) | (1U << 13U);
}

void uart2_write_char(char c)
{
    while ((USART2->SR & (1U << 7U)) == 0U)
    {
        // Wait until transmit data register is empty
    }

    USART2->DR = (uint8_t)c;
}

void uart2_write_string(const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        uart2_write_char(*str);
        str++;
    }
}
