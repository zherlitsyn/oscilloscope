/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern volatile uint16_t adc_data[2];
extern volatile uint32_t adc0_length;
extern volatile uint32_t adc1_length;
extern volatile uint8_t  adc0_filled;
extern volatile uint8_t  adc1_filled;
extern volatile uint8_t  adc_available;
extern volatile uint8_t  adc_reset_cyccnt;

extern volatile uint16_t adc_max[2];
extern volatile uint16_t adc_min[2];
extern volatile uint32_t adc_period[2];
extern volatile uint8_t  adc_period0_detected;
extern volatile uint8_t  adc_period1_detected;

extern uint32_t adc0_time[ADC_BUFFER_SIZE];
extern uint32_t adc1_time[ADC_BUFFER_SIZE];
extern uint16_t adc0[ADC_BUFFER_SIZE];
extern uint16_t adc1[ADC_BUFFER_SIZE];
extern uint8_t  adc_immediate;

extern uint8_t  trigger_mode;
extern uint16_t trigger0_value;
extern uint16_t trigger1_value;

extern uint8_t event_adc;
extern uint8_t event_button0;
extern uint8_t event_button1;
extern uint8_t event_button2;
extern uint8_t event_trigger0_detected;
extern uint8_t event_trigger1_detected;

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim10;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim10;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(BUTTON0_Pin);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(BUTTON2_Pin);
  HAL_GPIO_EXTI_IRQHandler(BUTTON1_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/* USER CODE BEGIN 1 */
volatile uint32_t adc0_time_delta = 0;
volatile uint32_t adc1_time_delta = 0;
volatile uint16_t adc0_prev       = 0;
volatile uint16_t adc1_prev       = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
    	if (adc_reset_cyccnt) {
    		DWT->CYCCNT = 0U;
    		adc0_time_delta = 0;
    		adc1_time_delta = 0;
    		adc_reset_cyccnt = 0;
    	}

    	if (!adc0_filled) {
			adc0_time[adc0_length] = (DWT->CYCCNT - adc0_time_delta) / (SystemCoreClock / 1000000);
			adc0[adc0_length] = adc_data[0] * ADC_CHANNEL0_SCALE;

			if (adc_max[0] < adc0[adc0_length])
				adc_max[0] = adc0[adc0_length];

			if (adc_min[0] > adc0[adc0_length])
				adc_min[0] = adc0[adc0_length];

			if (adc0_length < (ADC_BUFFER_SIZE - 1)) {

				uint8_t trigger = 0;

				if (trigger_mode == 0)
					trigger = (adc0_prev < trigger0_value && adc0[adc0_length] > trigger0_value);
				else
					trigger = (adc0_prev > trigger0_value && adc0[adc0_length] < trigger0_value);

				if (trigger && adc0_length != 0) {
					if (!event_trigger0_detected) {
						adc0_length = 0;
						adc0_time_delta = DWT->CYCCNT;
						event_trigger0_detected = 1;
					} else if (!adc_period0_detected) {
						adc_period[0] = adc0_time[adc0_length];
						adc_period0_detected = 1;
					}
				}

				adc0_prev = adc_data[0] * ADC_CHANNEL0_SCALE;
				adc0_length++;

			} else
				adc0_filled = 1;
    	}

    	if (!adc1_filled) {
			adc1_time[adc1_length] = (DWT->CYCCNT - adc1_time_delta) / (SystemCoreClock / 1000000);
			adc1[adc1_length] = adc_data[1] * ADC_CHANNEL1_SCALE;

			if (adc_max[1] < adc1[adc1_length])
				adc_max[1] = adc1[adc1_length];

			if (adc_min[1] > adc1[adc1_length])
				adc_min[1] = adc1[adc1_length];

			if (adc1_length < (ADC_BUFFER_SIZE - 1)) {

				uint8_t trigger = 0;

				if (trigger_mode == 0)
					trigger = (adc1_prev < trigger1_value && adc1[adc1_length] > trigger1_value);
				else
					trigger = (adc1_prev > trigger1_value && adc1[adc1_length] < trigger1_value);

				if (trigger && adc1_length != 0) {
					if (!event_trigger1_detected) {
						adc1_length = 0;
						adc1_time_delta = DWT->CYCCNT;
						event_trigger1_detected = 1;
					} else if (!adc_period1_detected) {
						adc_period[1] = adc1_time[adc1_length];
						adc_period1_detected = 1;
					}
				}

				adc1_prev = adc_data[1] * ADC_CHANNEL1_SCALE;
				adc1_length++;

			} else
				adc1_filled = 1;
    	}

		if (adc0_filled && adc1_filled) {
			event_adc = 1;

	    	if (!adc_immediate) {
				HAL_TIM_Base_Stop_IT(&htim10);
				adc_available = 1;
	    	}

    		return;
		}

		if (adc_immediate)
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data, 2);
		else
			adc_available = 1;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM10)
	{
		if (adc_available) {
			adc_available = 0;
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data, 2);
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BUTTON0_Pin)
		event_button0 = 1;

	if (GPIO_Pin == BUTTON1_Pin)
		event_button1 = 1;

	if (GPIO_Pin == BUTTON2_Pin)
		event_button2 = 1;
}

/* USER CODE END 1 */
