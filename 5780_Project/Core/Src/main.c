
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * @authors				: Tyler Liddel, Hyrum Saunders
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

/* ---------------- Includes ---------------- */
#include "main.h"
#include <stdio.h>

/* ---------------- Globals ---------------- */
uint16_t block;
uint16_t curr_x;
uint16_t curr_y;
uint8_t phase;
int pan = 2240, tilt = 3600;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	// Reset of all peripherals, Initializes the Flash interface and the Systick
	HAL_Init();

	// Configure the system clock
	SystemClock_Config();

	PWM_init();

	LED_init();

	block = 0;

	// UART
	// PC4: TX, PC5: RX
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	GPIOC->MODER |= (2 << 8) | (2 << 10);		  // Set to alternate function mode
	GPIOC->AFR[0] |= (1 << 16) | (1 << 20);		  // Set to correct AF mode for pin 4 and 5
	USART3->CR1 |= (1 << 3) | (1 << 2);			  // TX en, RX en
	USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200; // Target baud rate of 115200
	USART3->CR1 |= (1 << 5);					  // RXNE interrupt enable
	NVIC_EnableIRQ(USART3_4_IRQn);				  // Enable interrupt for USART 3/4
	NVIC_SetPriority(USART3_4_IRQn, 0);			  // Set priority for USART 3/4

	USART3->CR1 |= 1; // USART en

	while (1)
	{
		// Blue ticking means while loop running
		/*toggle_LED(BLUE);
		transmit_uart(curr_x >> 8);
		transmit_uart(curr_x);
		transmit_uart(curr_y);
		transmit_uart('\n');*/

		HAL_Delay(1);
	}
}

/*
 * All interrupts use the same interrupt handler. Must check status of flags before changing anything
 *
 */
void USART3_4_IRQHandler()
{
	// Receival complete
	GPIOC->ODR ^= (1 << 6);
	toggle_LED(BLUE);

	uint16_t byte = USART3->RDR;
	block = block >> 8;
	block |= (byte << 8);

	switch (phase)
	{
	case 0:
		if (block == 0xAA55)
			phase++;
		break;
	case 1:
		phase++;
		break;
	case 2:
		if (block == 0xAA55)
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
		track_object();
		phase = 0;
		break;
	default:
		phase++;
	}
}

void track_object(void)
{
	if (curr_x - 157 > 10 || 157 - curr_x > 10 || curr_y - 103 > 10 || 103 - curr_y > 10)
	{
		pan -= (curr_x - 157);
		tilt += (curr_y - 103);
		if (tilt < 850)
			tilt = 850;
		if (tilt > 4230)
			tilt = 4230;
		if (pan < 850)
			pan = 850;
		if (pan > 4230)
			pan = 4230;
		set_motor_pos(pan, tilt);
	}
}

void transmit_uart(uint8_t send)
{
	while (!(USART1->ISR & USART_ISR_TC))
		;

	USART3->TDR = (send & 0xFF);
	HAL_Delay(1);
}

void transmit_string_uart(uint8_t send[])
{
	int i = 0;
	while (send[i] != '\0')
	{
		transmit_uart(send[i]);
		i++;
	}
}

/**
 * @brief Connects PB4 and PB5 to TIM3_CH1 and TIM3_CH2 respectively
 *				 and configures the PWM to have a 50 Hz period
 */
void PWM_init(void)
{
	// Enable necessary clocks
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN_Msk;

	// Put PB4 and PB5 in AF1
	GPIOB->MODER |= (2 << 8) | (2 << 10);
	GPIOB->AFR[0] |= (1 << 16) | (1 << 20);

	/*
	 * Configure timer 3 to a 50 Hz Period
	 * with as large a ARR (range of values)
	 * as possible so I can have very precise
	 * duty cycles.
	 */
	TIM3->PSC = 4;
	TIM3->ARR = 40000;

	// Configure timer 3 output channels to PWM
	TIM3->CCMR1 |= (6 << 4);  // Set channel 1 to PWM mode 1 - In upcounting, channel 1 is active when TIMx_CNT < TIMx_CCR1
	TIM3->CCMR1 |= (6 << 12); // Set channel 2 to PWM mode 1 - In upcounting, channel 2 is active when TIMx_CNT < TIMx_CCR2

	// Output compare 1 and 2 preload enable. Read/Write operations go to preload register,
	// preload value loaded in active register at update events.
	TIM3->CCMR1 |= (1 << 3);  // Output compare 1 preload enable
	TIM3->CCMR1 |= (1 << 11); // Output compare 2 preload enable

	// Enable channel 1 and 2 in CCER
	TIM3->CCER |= 1;
	TIM3->CCER |= (1 << 4);

	/*
	 * Capture compare registers will start at 7.5%
	 * duty cycle (90 deg). The duty cycle should
	 * always stay between 5-10% (850-4230)
	 */
	TIM3->CCR1 = 2240; // PB4 Pan
	TIM3->CCR2 = 3600; // PB5 Tilt

	// Enable timer 3
	TIM3->CR1 = 1;
}

/**
 * @brief Sets the motor at a point between 0-180 degrees
 */
void set_motor_pos(int pan, int tilt)
{
	// TIM3->CCR1 = 1170 + (pan * 17);
	// TIM3->CCR2 = 1170 + (tilt * 17);
	TIM3->CCR1 = pan;
	TIM3->CCR2 = tilt;
}

/**
 * @brief This function sets up the LED GPIO pins
 */
void LED_init(void)
{
	// Enable clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN_Msk;

	// Set pins PC6, PC7, PC8, and PC9 (all LEDs) to general-purpose output mode
	GPIOC->MODER |= (1 << 12);
	GPIOC->MODER |= (1 << 14);
	GPIOC->MODER |= (1 << 16);
	GPIOC->MODER |= (1 << 18);

	// Set LEDs to push-pull, low speed, no pull-up/down resistors
	GPIOC->OTYPER = 0;
	GPIOC->OSPEEDR = 0;
	GPIOC->PUPDR = 0;
}

/**
 * @brief This function checks if an LED is currently high
 * @param LED_num (int): The number corresponding to the LED pin. Ex: PC6 would be 6.
 * @retval int: 1 if LED is high, 0 otherwise
 */
int LED_is_on(int LED_num)
{
	return GPIOC->ODR & (1 << LED_num);
}

/**
 * @brief This function sets the LED pin passed in to high
 * @param LED_num (int): The number corresponding to the LED pin. Ex: PC6 would be 6.
 * @retval None
 */
void turn_on_LED(int LED_num)
{
	GPIOC->ODR |= (1 << LED_num);
}

/**
 * @brief This function sets the LED pin passed in to low
 * @param LED_num (int): The number corresponding to the LED pin. Ex: PC6 would be 6.
 * @retval None
 */
void turn_off_LED(int LED_num)
{
	GPIOC->ODR &= ~(1 << LED_num);
}

/**
 * @brief This function toggles the LED pin passed in
 * @param LED_num (int): The number corresponding to the LED pin. Ex: PC6 would be 6.
 * @retval None
 */
void toggle_LED(int LED_num)
{
	if (LED_is_on(LED_num))
	{
		turn_off_LED(LED_num);
	}
	else
	{
		turn_on_LED(LED_num);
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

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
