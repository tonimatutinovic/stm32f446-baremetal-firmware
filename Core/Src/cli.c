/*
 * cli.c
 *
 *  Created on: 24. stu 2026.
 *      Author: tonim
 */

#include "cli.h"
#include "uart_driver.h"
#include "led.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static char command[16];
static uint8_t command_index = 0U;
static uint8_t command_overflow = 0U;

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
		} else {
			char response[64];
			snprintf(response, sizeof(response),
					"\r\nLED is BLINKING every %lu ms\r\n",
					(unsigned long) led_get_period());
			uart2_write_string(response);
		}
	} else {
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

}
