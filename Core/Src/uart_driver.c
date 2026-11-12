/*
 * uart_driver.c
 *
 *  Created on: 9. stu 2026.
 *      Author: tonim
 */

#include "uart_driver.h"
#include "stm32f446xx.h"

#define UART_RX_BUFFER_SIZE 8U

static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0U;
static volatile uint8_t rx_tail = 0U;
static volatile uint32_t rx_overflow_count = 0U;

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

    // PA3: Alternate Function mode
    GPIOA->MODER &= ~(3U << (2U * 3U));
    GPIOA->MODER |=  (2U << (2U * 3U));

    // PA3: AF7 = USART2_RX
    GPIOA->AFR[0] &= ~(0xFU << (4U * 3U));
    GPIOA->AFR[0] |=  (7U << (4U * 3U));

    // Enable USART2 clock
    RCC->APB1ENR |= (1U << 17U);
    (void)RCC->APB1ENR;

    // Baud rate: PCLK1 = 42 MHz, baud = 115200
    // Oversampling by 16
    USART2->BRR = 365U;

    // USART2 enable + transmitter + receiver
    USART2->CR1 |= (1U << 13U) | (1U << 3U) | (1U << 2U);

    // RXNEIE
    USART2->CR1 |= (1U << 5U);
    NVIC_EnableIRQ(USART2_IRQn);
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

char uart2_read_char(void)
{
    while ((USART2->SR & (1U << 5U)) == 0U)
    {
        // Wait for RXNE
    }

    return (char)(USART2->DR & 0xFFU);
}

void uart2_irq_handler(void)
{
    if ((USART2->SR & (1U << 5U)) != 0U)
    {
        uint8_t received_byte = (uint8_t)(USART2->DR & 0xFFU);

        uint8_t next_head =
            (rx_head + 1U) % UART_RX_BUFFER_SIZE;

        if (next_head != rx_tail)
        {
            rx_buffer[rx_head] = received_byte;
            rx_head = next_head;
        }
        else
        {
            rx_overflow_count++;
        }
    }
}

uint8_t uart2_read_available(char *c)
{
    if (c == 0)
    {
        return 0U;
    }

    NVIC_DisableIRQ(USART2_IRQn);

    if (rx_head == rx_tail)
    {
        NVIC_EnableIRQ(USART2_IRQn);
        return 0U;
    }

    *c = (char)rx_buffer[rx_tail];

    rx_tail = (rx_tail + 1U) % UART_RX_BUFFER_SIZE;

    NVIC_EnableIRQ(USART2_IRQn);

    return 1U;
}

uint32_t uart2_get_overflow_count(void)
{
    NVIC_DisableIRQ(USART2_IRQn);

    uint32_t count = rx_overflow_count;

    NVIC_EnableIRQ(USART2_IRQn);

    return count;
}
