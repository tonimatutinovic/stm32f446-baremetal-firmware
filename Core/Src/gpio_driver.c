/*
 * gpio_driver.c
 *
 *  Created on: 7. stu 2026.
 *      Author: tonim
 */


#include "gpio_driver.h"
#include "stm32f446xx.h"

static uint8_t gpio_enable_clock(GPIO_TypeDef *port){
	if(port == GPIOA)
		{
			// Enable GPIOA clock
			RCC->AHB1ENR |= (1U << 0);
		}
		else if(port == GPIOB){
			RCC->AHB1ENR |= (1U << 1);
		}
		else if(port == GPIOC){
				RCC->AHB1ENR |= (1U << 2);
			}
		else if(port == GPIOD){
				RCC->AHB1ENR |= (1U << 3);
			}
		else if(port == GPIOE){
					RCC->AHB1ENR |= (1U << 4);
				}
		else
		    {
		        return 0U;
		    }

	// Ensure the clock-enable write has completed
	(void)RCC->AHB1ENR;

	return 1U;
}


void gpio_init_output(GPIO_TypeDef *port, uint8_t pin)
{
	if (port == 0 || pin > 15U){
		return;
	}

	if(gpio_enable_clock(port) == 0U){
		return;
	}
    port->MODER &= ~(3U << (2U*pin));
    port->MODER |=  (1U << (2U*pin));
}

void gpio_set_pin(GPIO_TypeDef *port, uint8_t pin)
{
	if (port == 0 || pin > 15U)
	    {
	        return;
	    }
	// Set PA5 HIGH -> LED ON
    port->BSRR = (1U << pin);
}

void gpio_reset_pin(GPIO_TypeDef *port, uint8_t pin)
{
	if (port == 0 || pin > 15U)
	    {
	        return;
	    }
    port->BSRR = (1U << (pin+16U));
}

void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin){
	if (port == 0 || pin > 15U)
	    {
	        return;
	    }
	if (port->ODR & (1U << pin))
	{
	    gpio_reset_pin(port, pin);
	}
	else
	{
	    gpio_set_pin(port, pin);
	}
}

void gpio_write_pins(GPIO_TypeDef *port, uint16_t set_mask, uint16_t reset_mask)
{
    if (port == 0)
    {
        return;
    }

    port->BSRR = (uint32_t)set_mask | ((uint32_t)reset_mask << 16U);
}
