/*
 * systick_driver.h
 *
 *  Created on: 17. stu 2026.
 *      Author: tonim
 */

#ifndef INC_SYSTICK_DRIVER_H_
#define INC_SYSTICK_DRIVER_H_

#include <stdint.h>

void systick_tick_handler(void);
uint32_t systick_get_ms(void);

#endif /* INC_SYSTICK_DRIVER_H_ */
