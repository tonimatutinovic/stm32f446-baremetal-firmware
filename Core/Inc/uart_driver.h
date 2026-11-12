/*
 * uart_driver.h
 *
 *  Created on: 9. stu 2026.
 *      Author: tonim
 */

#ifndef INC_UART_DRIVER_H_
#define INC_UART_DRIVER_H_

#include <stdint.h>

void uart2_init(void);
void uart2_write_char(char c);
void uart2_write_string(const char *str);
char uart2_read_char(void);
void uart2_irq_handler(void);
uint8_t uart2_read_available(char *c);
uint32_t uart2_get_overflow_count(void);

#endif /* INC_UART_DRIVER_H_ */
