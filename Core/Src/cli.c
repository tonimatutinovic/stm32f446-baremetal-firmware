/*
 * cli.c
 *
 *  Created on: 24. stu 2026.
 *      Author: tonim
 */

#include "cli.h"
#include "uart_driver.h"
#include "led.h"
#include "adc_driver.h"
#include "pwm_driver.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static char command[16];
static uint8_t command_index = 0U;
static uint8_t command_overflow = 0U;

static uint8_t adc_pending = 0U;

void cli_init(void) {
	command_index = 0U;
	command_overflow = 0U;
}

static void cli_execute_command(void) {
	if (strcmp(command, "LED ON") == 0) {
		led_on();
		uart2_write_string("\r\nLED turned ON\r\n");
	} else if (strcmp(command, "LED OFF") == 0) {
		led_off();
		uart2_write_string("\r\nLED turned OFF\r\n");
	} else if (strncmp(command, "BLINK ", 6) == 0) {
		char *number_text = &command[6];
		char *endptr;

		errno = 0;
		unsigned long period = strtoul(number_text, &endptr, 10);

		if (*number_text >= '0' && *number_text <= '9' && endptr != number_text
				&& *endptr == '\0' &&
				errno != ERANGE && period >= 50U && period <= 10000U) {
			led_blink((uint32_t) period);

			char response[64];

			snprintf(response, sizeof(response),
					"\r\nBlinking every %lu ms\r\n", period);

			uart2_write_string(response);
		} else {
			uart2_write_string("\r\nInvalid blink period\r\n");
		}
	} else if (strcmp(command, "STATUS") == 0) {
		if (led_get_mode() == LED_MODE_ON) {
			uart2_write_string("\r\nLED is ON\r\n");
		} else if (led_get_mode() == LED_MODE_OFF) {
			uart2_write_string("\r\nLED is OFF\r\n");
		}
		else {
			char response[64];
			snprintf(response, sizeof(response),
					"\r\nLED is BLINKING every %lu ms\r\n",
					(unsigned long) led_get_period());
			uart2_write_string(response);
		}
	} else if (strcmp(command, "ADC READ") == 0) {
		if(adc_pending == 0U)
		{
			adc_pending = 1U;
			adc1_start_conversion();
		}
		else
		{
			uart2_write_string("\r\nADC busy\r\n");
		}
	} else if (strncmp(command, "PWM ", 4) == 0) {
		char *number_text = &command[4];
		char *endptr;

		errno = 0;
		unsigned long duty_percent = strtoul(number_text, &endptr, 10);

		if (*number_text >= '0' && *number_text <= '9' && endptr != number_text
				&& *endptr == '\0' &&
				errno != ERANGE && duty_percent <= 100U) {
			pwm_set_duty((uint8_t)duty_percent);

			char response[64];

			snprintf(response, sizeof(response),
					"\r\nPWM duty cycle: %lu%%\r\n", duty_percent);

			uart2_write_string(response);
		} else {
			uart2_write_string("\r\nInvalid duty cycle percentage\r\n");
		}
	}else {
		uart2_write_string("\r\nUnknown command!\r\n");
	}

}

void cli_process(void) {
	char c;

	if (uart2_read_available(&c) == 1U) {
		if (c == '\r' || c == '\n') {
			command[command_index] = '\0';
			if (command_overflow == 1U) {
				uart2_write_string("\r\nCommand too long\r\n");
			} else if (command_index > 0U) {
				cli_execute_command();
			}
			command_index = 0U;
			command_overflow = 0U;
		}

		else if (command_index < sizeof(command) - 1U) {
			command[command_index] = c;
			command_index++;
		} else {
			command_overflow = 1U;
		}

		uart2_write_char(c);

	}

	if(adc_pending == 1)
	{
		uint16_t adc_value;

		if(adc1_read_available(&adc_value))
		{
			adc_pending = 0;

			char response[64];

			uint16_t adc_voltage = ((uint32_t)adc_value * 3300) / 4095;

			snprintf(response, sizeof(response),
					"\r\nADC value: %u\r\nVoltage: %u mV\r\n", (unsigned int)adc_value, (unsigned int)adc_voltage);

			uart2_write_string(response);
		}
	}

}
