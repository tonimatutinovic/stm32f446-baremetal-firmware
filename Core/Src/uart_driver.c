/*
 * uart_driver.c
 *
 *  Created on: 9. stu 2026.
 *      Author: tonim
 */

#include "uart_driver.h"
#include "stm32f446xx.h"
#include <stddef.h>

#define UART_RX_BUFFER_SIZE 8U

static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0U;
static volatile uint8_t rx_tail = 0U;
static volatile uint32_t rx_overflow_count = 0U;
static volatile uint32_t rx_overrun_count = 0U;

#define UART_TX_BUFFER_SIZE 128U

static volatile uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint16_t tx_head = 0U;
static volatile uint16_t tx_tail = 0U;
static volatile uint32_t tx_overflow_count = 0U;

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

    NVIC_SetPriority(USART2_IRQn, 1U);
    NVIC_EnableIRQ(USART2_IRQn);
}

void uart2_write_char(char c)
{
    uart2_write((const uint8_t *)&c, 1U);
}

uint32_t uart2_write_string(const char *str)
{
    if (str == NULL)
    {
        return 0U;
    }

    uint32_t length = 0U;

    while (str[length] != '\0')
    {
        length++;
    }

    return uart2_write((const uint8_t *)str, length);
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
    uint32_t status = USART2->SR;

    if ((status & ((1U << 5U) | (1U << 3U))) != 0U)
    {
        uint8_t received_byte = (uint8_t)(USART2->DR & 0xFFU);

        if ((status & (1U << 3U)) != 0U)
        {
            rx_overrun_count++;
        }

        if ((status & (1U << 5U)) != 0U)
        {
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

    if (((USART2->SR & (1U << 7U)) != 0U) &&
        ((USART2->CR1 & (1U << 7U)) != 0U))
    {
        if (tx_head != tx_tail)
        {
            USART2->DR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1U) % UART_TX_BUFFER_SIZE;
        }
        else
        {
            USART2->CR1 &= ~(1U << 7U);
        }
    }
}

uint8_t uart2_read_available(char *c)
{
    if (c == NULL)
    {
        return 0U;
    }

    uint32_t was_enabled = NVIC_GetEnableIRQ(USART2_IRQn);
    NVIC_DisableIRQ(USART2_IRQn);

    uint8_t available = 0U;

    if (rx_head != rx_tail)
    {
        *c = (char)rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1U) % UART_RX_BUFFER_SIZE;
        available = 1U;
    }

    if (was_enabled)
    {
        NVIC_EnableIRQ(USART2_IRQn);
    }

    return available;
}

uint32_t uart2_get_overflow_count(void)
{
    return rx_overflow_count;;
}

uint32_t uart2_get_overrun_count(void)
{
    return rx_overrun_count;;
}

uint32_t uart2_write(const uint8_t *data, uint32_t length)
{
    if (data == NULL || length == 0U)
    {
        return 0U;
    }

    uint32_t was_enabled = NVIC_GetEnableIRQ(USART2_IRQn);

    NVIC_DisableIRQ(USART2_IRQn);

    uint32_t written = 0U;

    for (uint32_t i = 0U; i < length; i++)
    {
        uint16_t next_head = (tx_head + 1U) % UART_TX_BUFFER_SIZE;

        if (next_head == tx_tail)
        {
            break;
        }

        tx_buffer[tx_head] = data[i];
        tx_head = next_head;
        written++;
    }

    tx_overflow_count += length - written;

    if (written > 0U)
    {
        USART2->CR1 |= (1U << 7U);
    }

    if (was_enabled)
    {
        NVIC_EnableIRQ(USART2_IRQn);
    }

    return written;
}

uint32_t uart2_get_tx_overflow_count(void)
{
    return tx_overflow_count;
}
