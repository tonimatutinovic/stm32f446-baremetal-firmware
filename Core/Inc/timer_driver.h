/*
 * timer_driver.h
 *
 *  Created on: 14. stu 2026.
 *      Author: tonim
 */

#ifndef INC_TIMER_DRIVER_H_
#define INC_TIMER_DRIVER_H_

void tim2_init(void);
void tim2_irq_handler(void);
uint8_t tim2_take_tick(void);

#endif /* INC_TIMER_DRIVER_H_ */
