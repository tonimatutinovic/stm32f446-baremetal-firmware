/*
 * pwm_driver.h
 *
 *  Created on: 27. stu 2026.
 *      Author: tonim
 */

#ifndef INC_PWM_DRIVER_H_
#define INC_PWM_DRIVER_H_

#include <stdint.h>

void pwm_init(void);
void pwm_set_duty(uint8_t duty_percent);

#endif /* INC_PWM_DRIVER_H_ */
