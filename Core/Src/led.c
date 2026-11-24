/*
 * led.c
 *
 *  Created on: 24. stu 2026.
 *      Author: tonim
 */

#include "led.h"
#include "gpio_driver.h"
#include "timer_driver.h"

typedef struct
{
    LedMode mode;
    uint32_t period_ms;
    uint32_t last_toggle_ms;
} LedState;

static LedState led = {
    .mode = LED_MODE_OFF,
    .period_ms = 500U,
    .last_toggle_ms = 0U
};

void led_on(void)
{
	led.mode = LED_MODE_ON;
	gpio_set_pin(GPIOA, 5);
}

void led_off(void)
{
	led.mode = LED_MODE_OFF;
	gpio_reset_pin(GPIOA, 5);
}

LedMode led_get_mode(void)
{
	return led.mode;
}

uint32_t led_get_period(void)
{
	return led.period_ms;
}

void led_blink(uint32_t period_ms)
{
	led.mode = LED_MODE_BLINK;
	led.period_ms = period_ms;
	led.last_toggle_ms = tim2_get_ms();
}

void led_update(void)
{
	if(led.mode == LED_MODE_BLINK)
	{
		uint32_t now = tim2_get_ms();

		if((now - led.last_toggle_ms) >= led.period_ms){
			gpio_toggle_pin(GPIOA, 5);
			led.last_toggle_ms = now;
		}
	}
}

void led_init(void)
{
	gpio_init_output(GPIOA, 5);
	led_off();
	led.last_toggle_ms = 0U;
	led.period_ms = 500U;
}
