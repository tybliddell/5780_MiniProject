/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
	* @author					: Hyrum Saunders
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
#include "stm32f0xx_hal.h"

/* ---------------- Forward Declarations ---------------- */
void Error_Handler(void);
void SystemClock_Config(void);
void LED_init(void);
int LED_is_on(int);
void turn_on_LED(int);
void turn_off_LED(int);
void toggle_LED(int);

void PWM_init(void);
void set_motor_pos(int, int);

/* ---------------- Definitions ---------------- */
#define RED 6
#define BLUE 7
#define ORANGE 8
#define GREEN 9

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
