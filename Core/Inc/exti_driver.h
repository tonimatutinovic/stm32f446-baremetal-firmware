/*
 * exti_driver.h
 *
 *  Created on: 17. stu 2026.
 *      Author: tonim
 */

#ifndef INC_EXTI_DRIVER_H_
#define INC_EXTI_DRIVER_H_

void exti13_init(void);
void exti13_irq_handler(void);
uint8_t exti13_take_press(void);

#endif /* INC_EXTI_DRIVER_H_ */
