/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LEFT_ENCODER_A_Pin GPIO_PIN_0
#define LEFT_ENCODER_A_GPIO_Port GPIOA
#define LEFT_ENCODER_B_Pin GPIO_PIN_1
#define LEFT_ENCODER_B_GPIO_Port GPIOA
#define DEBUG_TX_Pin GPIO_PIN_2
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_3
#define DEBUG_RX_GPIO_Port GPIOA
#define RIGHT_ENCODER_A_Pin GPIO_PIN_6
#define RIGHT_ENCODER_A_GPIO_Port GPIOA
#define RIGHT_ENCODER_B_Pin GPIO_PIN_7
#define RIGHT_ENCODER_B_GPIO_Port GPIOA
#define TB6612_STBY_Pin GPIO_PIN_0
#define TB6612_STBY_GPIO_Port GPIOB
#define LEFT_AIN1_Pin GPIO_PIN_12
#define LEFT_AIN1_GPIO_Port GPIOB
#define LEFT_AIN2_Pin GPIO_PIN_13
#define LEFT_AIN2_GPIO_Port GPIOB
#define RIGHT_BIN1_Pin GPIO_PIN_14
#define RIGHT_BIN1_GPIO_Port GPIOB
#define RIGHT_BIN2_Pin GPIO_PIN_15
#define RIGHT_BIN2_GPIO_Port GPIOB
#define LEFT_PWM_Pin GPIO_PIN_8
#define LEFT_PWM_GPIO_Port GPIOA
#define RIGHT_PWM_Pin GPIO_PIN_9
#define RIGHT_PWM_GPIO_Port GPIOA
#define MPU6050_SCL_Pin GPIO_PIN_6
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_7
#define MPU6050_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
