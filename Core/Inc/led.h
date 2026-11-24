/*
 * led.h
 *
 *  Created on: 24. stu 2026.
 *      Author: tonim
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>

typedef enum
{
    LED_MODE_ON,
    LED_MODE_OFF,
    LED_MODE_BLINK
} LedMode;

void led_init(void);
void led_on(void);
void led_off(void);
void led_blink(uint32_t period_ms);
void led_update(void);

LedMode led_get_mode(void);
uint32_t led_get_period(void);

#endif /* INC_LED_H_ */
