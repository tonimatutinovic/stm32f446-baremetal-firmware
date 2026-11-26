/*
 * adc_driver.h
 *
 *  Created on: 25. stu 2026.
 *      Author: tonim
 */

#ifndef INC_ADC_DRIVER_H_
#define INC_ADC_DRIVER_H_

#include <stdint.h>

void adc1_init(void);
uint16_t adc1_read(void);
void adc_irq_handler(void);
void adc1_start_conversion(void);
uint8_t adc1_read_available(uint16_t *value);

#endif /* INC_ADC_DRIVER_H_ */
