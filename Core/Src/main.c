/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio_driver.h"
#include "uart_driver.h"
#include "timer_driver.h"
#include "exti_driver.h"
#include "systick_driver.h"
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum
  {
	  LED_MODE_ON,
	  LED_MODE_OFF,
	  LED_MODE_BLINK
  }LedMode;

  typedef struct
  {
	  LedMode mode;
	  uint32_t period_ms;
	  uint32_t last_toggle_ms;
  }LedState;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  // MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  gpio_init_output(GPIOA, 5);
  uart2_init();
  tim2_init();
  exti13_init();

  char command[16];
  int index = 0;
  char c;

  LedState led = {.mode = LED_MODE_OFF, .period_ms = 500U, .last_toggle_ms = 0U};

  uint8_t command_overflow = 0U;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(led.mode == LED_MODE_BLINK)
	  {
		  uint32_t now = tim2_get_ms();
		  if((now - led.last_toggle_ms) >= led.period_ms){
			  gpio_toggle_pin(GPIOA, 5);
			  led.last_toggle_ms = now;
		  }
	  }

	  // Handle UART received bytes
	  if (uart2_read_available(&c) == 1U)
	  {
		  if(c == '\r' || c == '\n')
		  {
			  command[index] = '\0';
			  if (command_overflow == 1U)
			  {
				  uart2_write_string("\r\nCommand too long\r\n");
			  }
			  else if(index > 0U){
				  if(strcmp(command, "LED ON") == 0)
				  {
					  led.mode = LED_MODE_ON;
					  gpio_set_pin(GPIOA, 5);
					  uart2_write_string("\r\nLED turned ON\r\n");
				  }
				  else if(strcmp(command, "LED OFF") == 0)
				  {
					  led.mode = LED_MODE_OFF;
					  gpio_reset_pin(GPIOA, 5);
					  uart2_write_string("\r\nLED turned OFF\r\n");
				  }
				  else if (strncmp(command, "BLINK ", 6) == 0)
				  {
				      char *number_text = &command[6];
				      char *endptr;

				      errno = 0;
				      unsigned long period = strtoul(number_text, &endptr, 10);

				      if (*number_text >= '0' && *number_text <= '9' &&
				           endptr != number_text && *endptr == '\0' &&
				           errno != ERANGE &&
				           period >= 50U && period <= 10000U)
				      {
				          led.mode = LED_MODE_BLINK;
				          led.period_ms = (uint32_t)period;
				          led.last_toggle_ms = tim2_get_ms();

				          char response[64];

				          snprintf(response, sizeof(response),
				                   "\r\nBlinking every %lu ms\r\n", period);

				          uart2_write_string(response);
				      }
				      else
				      {
				          uart2_write_string("\r\nInvalid blink period\r\n");
				      }
				  }
				  else if (strcmp(command, "STATUS") == 0)
				  {
					  if (led.mode == LED_MODE_ON)
					  {
						  uart2_write_string("\r\nLED is ON\r\n");
					  }
					  else if (led.mode == LED_MODE_OFF)
					  {
						  uart2_write_string("\r\nLED is OFF\r\n");
					  }
					  else
					  {
						  char response[64];
						  snprintf(response, sizeof(response), "\r\nLED is BLINKING every %lu ms\r\n", (unsigned long)led.period_ms);
						  uart2_write_string(response);
					  }
				  }
				  else
				  {
					  uart2_write_string("\r\nUnknown command!\r\n");
				  }

			  }

			  index = 0U;
			  command_overflow = 0U;
		  }

		  else if(index < sizeof(command) - 1U)
		  {
			  command[index] = c;
			  index++;
		  }
		  else
		  {
			  command_overflow = 1U;
		  }

		  uart2_write_char(c);

	  }


	  // EXTI - button press
	  if (exti13_take_press())
	  {
	      uart2_write_string("Button pressed!\r\n");
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
