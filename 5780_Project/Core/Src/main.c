/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>

void SystemClock_Config(void);

uint16_t block;
uint16_t curr_x;
uint16_t curr_y;
uint8_t phase;

void transmit_string_uart(uint8_t send[]);
void transmit_uart(uint8_t send);

/*
* All interrupts use the same interrupt handler. Must check status of flags before changing anything
*
*/
void USART3_4_IRQHandler()
{
	// Receival complete
	GPIOC->ODR ^= (1 << 6);

	uint16_t byte = USART3->RDR;
	block = block >> 8;
	block |= (byte << 8);
	
	switch(phase)
	{
		case 0:
			if(block == 0xAA55)
				phase++;
			break;
		case 1:
			phase++;
			break;
		case 2:
			if(block == 0xAA55)
				phase++;
			else
				phase = 0;
			break;
		case 8:
			// The x center has been receieved
			curr_x = block;
			phase++;
			break;
		case 10:
			// The y center has been received
			curr_y = block;
			phase++;
			break;
		case 14:
			// Final piece of block
			phase = 0;
			break;
		default:
		  phase++;		
	}
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();

	SystemClock_Config();

	block = 0;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	
	// LED
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable GPIOC peripheral clock
	
	GPIOC->MODER |= (1 << 12) | (1 << 14) | (1 << 16) | (1 << 18); // Set MODER to general-purpose output mode
	GPIOC->OTYPER  &= ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9)); // Set OTYPER to push-pull output type
	GPIOC->OSPEEDR &= ~((3 << 12) | (3 << 14) | (3 << 16) | (3 << 18)); // Set OSPEEDR to low speed
	GPIOC->PUPDR &= ~((3 << 12) | (3 << 14) | (3 << 16) | (3 << 18)); // Set PUPDR to no pull-up pull-down 			
	
	// UART
	// PC4: TX, PC5: RX
	GPIOC->MODER |= (2 << 8) | (2 << 10); // Set to alternate function mode
	GPIOC->AFR[0] |= (1 << 16)| (1 << 20); // Set to correct AF mode for pin 4 and 5
	USART3->CR1 |= (1 << 3) | (1 << 2); // TX en, RX en
	USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200; // Target baud rate of 115200
	USART3->CR1 |= (1 << 5); // RXNE interrupt enable
	NVIC_EnableIRQ(USART3_4_IRQn); // Enable interrupt for USART 3/4
	NVIC_SetPriority(USART3_4_IRQn, 0); // Set priority for USART 3/4
	
	USART3->CR1 |= 1; // USART en

  while (1)
  {
		// Blue ticking means while loop running
		GPIOC->ODR ^= (1 << 7);
		transmit_uart(curr_x >> 8);
		HAL_Delay(1);
		transmit_uart('\0');
		HAL_Delay(1);
		transmit_uart(curr_x);
		HAL_Delay(1);
		transmit_uart('\0');
		HAL_Delay(1);
		transmit_uart(curr_y);
		HAL_Delay(1);
		transmit_uart('\n');
		HAL_Delay(50);
  }
}

void transmit_uart(uint8_t send) 
{
	while(!(USART1->ISR & USART_ISR_TC));

	USART3->TDR = (send & 0xFF);
}

void transmit_string_uart(uint8_t send[])
{
	int i = 0;
	while(send[i] != '\0')
	{
		transmit_uart(send[i]);
		HAL_Delay(1);
		i++;
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

#ifdef  USE_FULL_ASSERT
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

