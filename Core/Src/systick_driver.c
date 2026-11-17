/*
 * systick_driver.c
 *
 *  Created on: 17. stu 2026.
 *      Author: tonim
 */

#include "systick_driver.h"

static volatile uint32_t milliseconds = 0U;

void systick_tick_handler(void)
{
    milliseconds++;
}

uint32_t systick_get_ms(void)
{
    return milliseconds;
}
