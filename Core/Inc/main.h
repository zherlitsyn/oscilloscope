/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_CH3_Pin GPIO_PIN_0
#define ADC_CH3_GPIO_Port GPIOA
#define ILI9341_RESET_Pin GPIO_PIN_2
#define ILI9341_RESET_GPIO_Port GPIOA
#define ILI9341_DC_Pin GPIO_PIN_3
#define ILI9341_DC_GPIO_Port GPIOA
#define ILI9341_CS_Pin GPIO_PIN_4
#define ILI9341_CS_GPIO_Port GPIOA
#define ADC_CH1_Pin GPIO_PIN_0
#define ADC_CH1_GPIO_Port GPIOB
#define ADC_CH2_Pin GPIO_PIN_1
#define ADC_CH2_GPIO_Port GPIOB
#define PWM_GENERATOR_Pin GPIO_PIN_8
#define PWM_GENERATOR_GPIO_Port GPIOA
#define BUTTON2_Pin GPIO_PIN_11
#define BUTTON2_GPIO_Port GPIOA
#define BUTTON2_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON1_Pin GPIO_PIN_15
#define BUTTON1_GPIO_Port GPIOA
#define BUTTON1_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON0_Pin GPIO_PIN_3
#define BUTTON0_GPIO_Port GPIOB
#define BUTTON0_EXTI_IRQn EXTI3_IRQn
#define ENCODER1_INB_Pin GPIO_PIN_4
#define ENCODER1_INB_GPIO_Port GPIOB
#define ENCODER1_INA_Pin GPIO_PIN_5
#define ENCODER1_INA_GPIO_Port GPIOB
#define ENCODER0_INB_Pin GPIO_PIN_6
#define ENCODER0_INB_GPIO_Port GPIOB
#define ENCODER0_INA_Pin GPIO_PIN_7
#define ENCODER0_INA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define ADC_BUFFER_SIZE 512
#define ADC_CHANNEL0_SCALE 11
#define ADC_CHANNEL1_SCALE 1

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
