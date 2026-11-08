/*
 * gpio_driver.h
 *
 *  Created on: 7. stu 2026.
 *      Author: tonim
 */

#ifndef INC_GPIO_DRIVER_H_
#define INC_GPIO_DRIVER_H_

#include <stdint.h>
#include "stm32f446xx.h"

void gpio_init_output(GPIO_TypeDef *port, uint8_t pin);
void gpio_set_pin(GPIO_TypeDef *port, uint8_t pin);
void gpio_reset_pin(GPIO_TypeDef *port, uint8_t pin);
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);
void gpio_write_pins(GPIO_TypeDef *port, uint16_t set_mask, uint16_t reset_mask);

#endif /* INC_GPIO_DRIVER_H_ */
