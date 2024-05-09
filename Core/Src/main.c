/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ili9341.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim10;

/* USER CODE BEGIN PV */
ILI9341TypeDef display;

volatile uint16_t adc_data[3] = { 0 };
volatile uint32_t adc0_length = 0;
volatile uint32_t adc1_length = 0;
volatile uint8_t  adc0_filled = 0;
volatile uint8_t  adc1_filled = 1;
volatile uint8_t  adc_available = 1;
volatile uint8_t  adc_reset_cyccnt = 1;

volatile uint16_t adc_max[2]    = {  0 };
volatile uint16_t adc_min[2]    = { -1 };
volatile uint32_t adc_period[2] = {  0 };
volatile uint8_t  adc_period0_detected = 0;
volatile uint8_t  adc_period1_detected = 0;

uint32_t adc0_time[ADC_BUFFER_SIZE];
uint32_t adc1_time[ADC_BUFFER_SIZE];
uint16_t adc0[ADC_BUFFER_SIZE];
uint16_t adc1[ADC_BUFFER_SIZE];
uint8_t  adc_immediate = 1;

uint32_t xlim_us = 200;
uint32_t ylim_uV = 1000000;

uint16_t cursor0 = 120;
uint16_t cursor1 = 196;

uint16_t trigger0 = 70;
uint16_t trigger1 = 146;

uint8_t  trigger_mode   = 0;
uint16_t trigger0_value = 2048;
uint16_t trigger1_value = 2048;

uint8_t event_adc      = 0;
uint8_t event_axis     = 1;
uint8_t event_mode     = 1;
uint8_t event_cursor   = 1;
uint8_t event_trigger  = 1;
uint8_t event_channel  = 1;
uint8_t event_seconds  = 1;
uint8_t event_voltage  = 1;
uint8_t event_button0  = 0;
uint8_t event_button1  = 0;
uint8_t event_button2  = 0;
uint8_t event_selector = 1;
uint8_t event_trigger_mode = 1;
uint8_t event_trigger0_detected = 1;
uint8_t event_trigger1_detected = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM10_Init(void);
/* USER CODE BEGIN PFP */
static void drawAxis(ILI9341TypeDef *display);
static void clearCursor(ILI9341TypeDef *display, uint16_t pos);
static void clearTrigger(ILI9341TypeDef *display, uint16_t pos);
static void drawCursor(ILI9341TypeDef *display, uint16_t pos, char *name, uint16_t color);
static void drawTrigger(ILI9341TypeDef *display, uint16_t pos, char *name, uint16_t color);
static void drawSignal(ILI9341TypeDef *display, uint32_t *adc_time, uint16_t *adc0, uint32_t adc_length, uint16_t pixel_dirty[280][2], uint16_t cursor, uint16_t color);
static void clearSignal(ILI9341TypeDef *display, uint16_t pixel_dirty[280][2]);
static void drawSignalParam(ILI9341TypeDef *display, char *string, size_t size, uint16_t adc_max, uint16_t adc_min, uint32_t adc_period);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */

  // For DWT->CYCCNT ...
  SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;

  // Display init
  display.spi             = &hspi1;
  display.cs_gpio_port    = ILI9341_CS_GPIO_Port;
  display.dc_gpio_port    = ILI9341_DC_GPIO_Port;
  display.reset_gpio_port = ILI9341_RESET_GPIO_Port;
  display.cs_pin          = ILI9341_CS_Pin;
  display.dc_pin          = ILI9341_DC_Pin;
  display.reset_pin       = ILI9341_RESET_Pin;
  display.width           = 320;
  display.height          = 240;
  display.orientation     = ILI9341_ORIENTATION_ROTATE_RIGHT;

  ILI9341_UNSELECT(&display);
  ILI9341_Init(&display);

  // Local vars
  char string[255];

  uint8_t  menu_extended         = 0;
  uint8_t  menu_channel0_enabled = 1;
  uint8_t  menu_channel1_enabled = 0;
  int8_t   menu_selected_item    = 2;
  uint16_t menu_selector_x       = 96;
  uint16_t menu_selector_y       = 3;

  uint8_t  mode = 0;
  uint8_t  mode_seconds = 5;
  uint8_t  mode_voltage = 6;

  uint16_t encoder0_prev = 0;
  uint16_t encoder1_prev = 0;

  uint8_t  frames = 0;
  uint32_t frames_ticks = HAL_GetTick();

  uint16_t pixel_dirty0[280][2];
  uint16_t pixel_dirty1[280][2];
  for (uint16_t i = 0; i < 280; i++) {
	  pixel_dirty0[i][0] = 0;
	  pixel_dirty0[i][1] = 0;
	  pixel_dirty1[i][0] = 0;
	  pixel_dirty1[i][1] = 0;
  }

  // Dispaly freq. (for debug)
  ILI9341_FillScreen(&display, ILI9341_BLACK);

  snprintf(string, 255, "Oscilloscope");
  ILI9341_WriteString(&display, 0, 18 * 0, string, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);

  snprintf(string, 255, "SYCLK = %ldMHz", HAL_RCC_GetSysClockFreq()/1000000);
  ILI9341_WriteString(&display, 0, 18 * 1, string, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);

  snprintf(string, 255, "HCLK  = %ldMHz", HAL_RCC_GetHCLKFreq()/1000000);
  ILI9341_WriteString(&display, 0, 18 * 2, string, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);

  snprintf(string, 255, "APB1  = %ldMHz", HAL_RCC_GetPCLK1Freq()/1000000);
  ILI9341_WriteString(&display, 0, 18 * 3, string, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);

  snprintf(string, 255, "APB2  = %ldMHz", HAL_RCC_GetPCLK2Freq()/1000000);
  ILI9341_WriteString(&display, 0, 18 * 4, string, Font_11x18, ILI9341_BLACK, ILI9341_WHITE);

  HAL_Delay(1000);
  ILI9341_FillScreen(&display, ILI9341_BLACK);

  adc_reset_cyccnt = 1;
  if (adc_immediate) {
	  // The ADC starts immediately after the previous measurement is handled
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data, 2);
  } else {
	  // ADC starts by timer
	  HAL_TIM_Base_Start_IT(&htim10);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	// Draw axis
	if (event_axis) {
		drawAxis(&display);
		event_axis = 0;
	}

	uint8_t local_event_adc = 0;
	if (event_adc) {
		local_event_adc = 1;
		event_adc = 0;
	}

	// Draw signals
	if (local_event_adc) {

		if (menu_channel0_enabled)
			drawSignal(&display, adc0_time, adc0, adc0_length, pixel_dirty0, cursor0, ILI9341_YELLOW);

		if (menu_channel1_enabled)
			drawSignal(&display, adc1_time, adc1, adc1_length, pixel_dirty1, cursor1, ILI9341_CYAN);

	}

	// Draw FPS
	if (frames > 60) {
		snprintf(string, 255, "FPS: %5i", (int)(1000.0f / ((float)(HAL_GetTick() - frames_ticks) / 60.0f)));
		ILI9341_WriteString(&display, 225, 5, string, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);

		frames = 0;
		frames_ticks = HAL_GetTick();
	}

	// Handle button events
	if (event_button0) {

		if (!menu_extended) {
			ILI9341_Rectangle(&display, menu_selector_x, menu_selector_y, 38, 13, ILI9341_BLACK);

			menu_selected_item++;

			if (menu_selected_item > 3)
				menu_selected_item = 2;

			menu_selector_x = 20 + 38 * menu_selected_item;
			menu_selector_y = 3;

			event_selector = 1;
		}

		event_button0 = 0;
	}

	if (event_button1) {
		menu_extended = !menu_extended;

		if (!menu_extended) {
			ILI9341_Rectangle(&display, menu_selector_x, menu_selector_y, 38, 13, ILI9341_BLACK);

			menu_selected_item = 2;

			menu_selector_x = 20 + 38 * menu_selected_item;
			menu_selector_y = 3;

			mode = 0;
			event_selector = 1;
		} else
			mode = 1;

		event_mode = 1;
		event_button1 = 0;
	}

	if (event_button2) {
		if (menu_selected_item == 2) {
			menu_channel0_enabled = !menu_channel0_enabled;

			if (!menu_channel0_enabled)
				clearSignal(&display, pixel_dirty0);
		}

		if (menu_selected_item == 3) {
			menu_channel1_enabled = !menu_channel1_enabled;

			if (!menu_channel1_enabled)
				clearSignal(&display, pixel_dirty1);
		}

		event_cursor  = 1;
		event_trigger = 1;
		event_channel = 1;
		event_button2 = 0;
	}

	// Handle UI redraw channel events
	if (event_channel) {
		uint16_t channel0_color = menu_channel0_enabled ? ILI9341_YELLOW  : ILI9341_COLOR565(60, 60, 0);
		uint16_t channel1_color = menu_channel1_enabled ? ILI9341_CYAN    : ILI9341_COLOR565(0, 60, 60);

		ILI9341_WriteString(&display, 110 - 12, 5, " CH1 ", Font_7x10, ILI9341_BLACK, channel0_color);
		ILI9341_WriteString(&display, 148 - 12, 5, " CH2 ", Font_7x10, ILI9341_BLACK, channel1_color);

		event_channel = 0;
	}

	// Handle encoder0
	int32_t encoder0_curr = __HAL_TIM_GET_COUNTER(&htim3);
	encoder0_curr = 32767 - ((encoder0_curr - 1) & 0xFFFF) / 2;

	if(encoder0_curr != encoder0_prev) {
		int32_t delta = encoder0_curr - encoder0_prev;

		if (delta > 10)
			delta = -1;

		if (delta < -10)
			delta = 1;

		if (menu_extended) {
			ILI9341_Rectangle(&display, menu_selector_x, menu_selector_y, 38, 13, ILI9341_BLACK);

			menu_selected_item += delta;

			if (menu_selected_item < 0)
				menu_selected_item = 0;

			if (menu_selected_item > 5)
				menu_selected_item = 5;

			if (menu_selected_item < 4) {
				menu_selector_x = 20 + 38 * menu_selected_item;
				menu_selector_y = 3;
			} else {
				menu_selector_x = 20 + 38 * (menu_selected_item - 4);
				menu_selector_y = 223;
			}

			event_selector = 1;

		} else {

			if (menu_selected_item == 2) {
				clearCursor(&display, cursor0);
				cursor0 += delta;

				if (cursor0 < 20)
					cursor0 = 20;

				if (cursor0 > 220)
					cursor0 = 220;
			}

			if (menu_selected_item == 3) {
				clearCursor(&display, cursor1);
				cursor1 += delta;

				if (cursor1 < 20)
					cursor1 = 20;

				if (cursor1 > 220)
					cursor1 = 220;
			}

		}

		event_cursor = 1;
		event_trigger = 1;
		encoder0_prev = encoder0_curr;
	}

	// Handle UI redraw cursor events
	if (event_cursor) {
		clearCursor(&display, cursor0);
		clearCursor(&display, cursor1);

		if (menu_channel0_enabled)
			drawCursor(&display, cursor0, "C1", ILI9341_YELLOW);

		if (menu_channel1_enabled)
			drawCursor(&display, cursor1, "C2", ILI9341_CYAN);

		event_cursor = 0;
	}

	// Handle encoder1
	int32_t encoder1_curr = __HAL_TIM_GET_COUNTER(&htim4);
	encoder1_curr = 32767 - ((encoder1_curr - 1) & 0xFFFF) / 2;

	if(encoder1_curr != encoder1_prev || event_trigger) {
		int32_t delta = encoder1_curr - encoder1_prev;

		if (delta > 10)
			delta = -1;

		if (delta < -10)
			delta = 1;

		if (menu_extended) {

			if (menu_selected_item == 1) {
				trigger_mode += delta;

				if (trigger_mode < 0)
					trigger_mode = 0;

				if (trigger_mode > 1)
					trigger_mode = 1;

				event_trigger_mode = 1;
			}

			if (menu_selected_item == 4) {
				mode_seconds += delta;

				if (mode_seconds < 0)
					mode_seconds = 0;

				if (mode_seconds > 16)
					mode_seconds = 16;

				uint32_t list_seconds[17] = {
					5,
					10,
					20,
					50,
					100,
					200,
					500,
					1000,
					2000,
					5000,
					10000,
					20000,
					50000,
					100000,
					200000,
					500000,
					1000000
				};

				xlim_us = list_seconds[mode_seconds];

				if (adc_immediate) {
					HAL_TIM_Base_Stop_IT(&htim10);
					adc_available = 1;
				}

				HAL_ADC_Stop_DMA(&hadc1);

				adc_reset_cyccnt = 1;
				adc0_length = 0;
				adc1_length = 0;
				adc_max[0] = 0;
				adc_max[1] = 0;
				adc_min[0] = -1;
				adc_min[1] = -1;
				adc_period[0] = 0;
				adc_period[1] = 0;
				adc_period0_detected = 0;
				adc_period1_detected = 0;
				event_trigger0_detected = 0;
				event_trigger1_detected = 0;

				if (menu_channel0_enabled)
					adc0_filled = 0;
				else
					adc0_filled = 1;

				if (menu_channel1_enabled)
					adc1_filled = 0;
				else
					adc1_filled = 1;

				adc_immediate = xlim_us <= 500;

				if (!adc_immediate) {
					uint32_t list_timer_settings[17][2] = {
					//  { Prescaler, Period }
						{     0,   0 },
						{     0,   0 },
						{     0,   0 },
						{     0,   0 },
						{     0,   0 },
						{     0,   0 },
						{     0,   0 },
						{    40, 100 },
						{    81, 100 },
						{   205, 100 },
						{   410, 100 },
						{   822, 100 },
						{  2056, 100 },
						{  4113, 100 },
						{  8228, 100 },
						{ 20570, 100 },
						{ 41142, 100 }
					};

					htim10.Init.Prescaler = list_timer_settings[mode_seconds][0];
					htim10.Init.Period    = list_timer_settings[mode_seconds][1];

					if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
						Error_Handler();

					HAL_TIM_Base_Start_IT(&htim10);
				} else
					HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data, 2);

				local_event_adc = 0;
				event_seconds = 1;
			}

			if (menu_selected_item == 5) {
				mode_voltage += delta;

				if (mode_voltage < 0)
					mode_voltage = 0;

				if (mode_voltage > 9)
					mode_voltage = 9;

				uint32_t list_voltage[10] = {
					10000,
					20000,
					50000,
					100000,
					200000,
					500000,
					1000000,
					2000000,
					5000000,
					10000000
				};

				ylim_uV = list_voltage[mode_voltage];

				event_voltage = 1;
			}

		} else {

			if (menu_selected_item == 2) {
				clearTrigger(&display, trigger0);
				trigger0 += delta;

				if (trigger0 < 20)
					trigger0 = 20;

				if (trigger0 > 220)
					trigger0 = 220;

				if (trigger0 > cursor0)
					trigger0 = cursor0;

				float uV = -((float)(ylim_uV) * ((8.0f * (trigger0 - cursor0)) / 200.0f));
				trigger0_value = uV * 4096.0f / 3300000.0f;
			}

			if (menu_selected_item == 3) {
				clearTrigger(&display, trigger1);
				trigger1 += delta;

				if (trigger1 < 20)
					trigger1 = 20;

				if (trigger1 > 220)
					trigger1 = 220;

				if (trigger1 > cursor1)
					trigger1 = cursor1;

				float uV = -((float)(ylim_uV) * ((8.0f * (trigger1 - cursor1)) / 200.0f));
				trigger1_value = uV * 4096.0f / 3300000.0f;
			}

		}

		event_trigger = 1;
		encoder1_prev = encoder1_curr;
	}

	// Handle UI redraw trigger events
	if (event_trigger) {
		clearTrigger(&display, trigger0);
		clearTrigger(&display, trigger1);

		if (menu_channel0_enabled)
			drawTrigger(&display, trigger0, "T1", ILI9341_YELLOW);

		if (menu_channel1_enabled)
			drawTrigger(&display, trigger1, "T2", ILI9341_CYAN);

		event_trigger = 0;
	}

	// Handle UI redraw selector events
	if (event_selector) {
		ILI9341_Rectangle(&display, menu_selector_x, menu_selector_y, 38, 13, ILI9341_WHITE);
		event_selector = 0;
	}

	// Handle UI redraw mode events
	if (event_mode) {
		if (mode == 0)
			ILI9341_WriteString(&display, 22, 5, " RUN ", Font_7x10, ILI9341_BLACK, ILI9341_GREEN);

		if (mode == 1)
			ILI9341_WriteString(&display, 22, 5, "MENU:", Font_7x10, ILI9341_BLACK, ILI9341_BLUE);

		if (mode == 2)
			ILI9341_WriteString(&display, 22, 5, "HOLD:", Font_7x10, ILI9341_BLACK, ILI9341_YELLOW);

		event_mode = 0;
	}

	// Handle UI redraw trigger mode events
	if (event_trigger_mode) {
		ILI9341_FillRectangle(&display, 61,  5, 33, 10, ILI9341_BLACK);
		ILI9341_FillRectangle(&display, 61, 14, 11, 1, ILI9341_WHITE);
		ILI9341_FillRectangle(&display, 72,  5, 11, 1, ILI9341_WHITE);
		ILI9341_FillRectangle(&display, 83, 14, 11, 1, ILI9341_WHITE);
		ILI9341_FillRectangle(&display, 72,  5, 1, 10, ILI9341_WHITE);
		ILI9341_FillRectangle(&display, 82,  5, 1, 10, ILI9341_WHITE);

		if (trigger_mode == 0) {
			for (uint8_t i = 0; i < 4; i++) {
				for (uint8_t j = i; j < (7 - i); j++)
					ILI9341_DrawPixel(&display, 69 + j, 11 - i, ILI9341_GREEN);
			}
		} else {
			for (uint8_t i = 0; i < 4; i++) {
			  for (uint8_t j = i; j < (7 - i); j++)
				  ILI9341_DrawPixel(&display, 79 + j, 8 + i, ILI9341_RED);
			}
		}

		event_trigger_mode = 0;
	}

	// Handle UI redraw seconds events
	if (event_seconds) {
		if (xlim_us >= 1000000)
			snprintf(string, 255, "%3li s",  xlim_us / 1000000);
		else if (xlim_us >= 1000)
			snprintf(string, 255, "%3lims", xlim_us / 1000);
		else
			snprintf(string, 255, "%3lius", xlim_us);

		ILI9341_WriteString(&display, 22, 225, string, Font_7x10, ILI9341_BLACK, ILI9341_WHITE);

		event_seconds = 0;
	}

	// Handle UI redraw voltage events
	if (event_voltage) {
		if (ylim_uV >= 1000000)
			snprintf(string, 255, "%3li V",  ylim_uV / 1000000);
		else if (ylim_uV >= 1000)
			snprintf(string, 255, "%3limV", ylim_uV / 1000);
		else
			snprintf(string, 255, "%3liuV", ylim_uV);

		ILI9341_WriteString(&display, 60, 225, string, Font_7x10, ILI9341_BLACK, ILI9341_WHITE);

		event_voltage = 0;
	}

	snprintf(string, 255, "T:%i%i", event_trigger0_detected, event_trigger1_detected);
	ILI9341_WriteString(&display, 195, 5, string, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);

	if (menu_channel1_enabled && menu_selected_item == 3) {
		//if (adc_period1_detected)
			drawSignalParam(&display, string, 255, adc_max[1], adc_min[1], adc_period[1]);
	} else if (menu_channel0_enabled) {
		//if (adc_period0_detected)
			drawSignalParam(&display, string, 255, adc_max[0], adc_min[0], adc_period[0]);
	}

	// Restart ADC ...
	if (local_event_adc) {
		adc_reset_cyccnt = 1;
		adc0_length = 0;
		adc1_length = 0;
		adc_max[0] = 0;
		adc_max[1] = 0;
		adc_min[0] = -1;
		adc_min[1] = -1;
		adc_period[0] = 0;
		adc_period[1] = 0;
		adc_period0_detected = 0;
		adc_period1_detected = 0;
		event_trigger0_detected = 0;
		event_trigger1_detected = 0;

		if (menu_channel0_enabled)
			adc0_filled = 0;
		else
			adc0_filled = 1;

		if (menu_channel1_enabled)
			adc1_filled = 0;
		else
			adc1_filled = 1;

		if (adc_immediate) {
			// The ADC starts immediately after the previous measurement is handled
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_data, 2);
		} else {
			// ADC starts by timer
			HAL_TIM_Base_Start_IT(&htim10);
		}

		local_event_adc = 0;
	}

	frames++;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  // Signal [Hz] = (1 / (Period * (1 / (96000000 / Prescaler))))
  //
  // 100Hz:
  //   htim1.Init.Prescaler = 95;
  //   htim1.Init.Period = 9999;
  //   sConfigOC.Pulse = 5000;
  //
  // 1kHz:
  //   htim1.Init.Prescaler = 95;
  //   htim1.Init.Period = 999;
  //   sConfigOC.Pulse = 500;
  //
  // 10kHz:
  //   htim1.Init.Prescaler = 95;
  //   htim1.Init.Period = 99;
  //   sConfigOC.Pulse = 50;

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 95;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 40;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 100;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  HAL_GPIO_WritePin(GPIOA, ILI9341_RESET_Pin|ILI9341_DC_Pin|ILI9341_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ILI9341_RESET_Pin ILI9341_DC_Pin ILI9341_CS_Pin */
  GPIO_InitStruct.Pin = ILI9341_RESET_Pin|ILI9341_DC_Pin|ILI9341_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON2_Pin BUTTON1_Pin */
  GPIO_InitStruct.Pin = BUTTON2_Pin|BUTTON1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON0_Pin */
  GPIO_InitStruct.Pin = BUTTON0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void drawAxis(ILI9341TypeDef *display)
{
	for (uint16_t i = 0; i < 9; i++) {
		uint16_t y = 20 + 25 * i;

		if (i == 0 || i == 8) {
			ILI9341_FillRectangle(display, 20, y, 276, 1, ILI9341_WHITE);
			continue;
		}

		for (uint16_t j = 0; j < 276; j += 2)
			ILI9341_DrawPixel(display, 20 + j, y, ILI9341_GRAY);
	}

	for (uint16_t i = 0; i < 12; i++) {
		uint16_t x = 20 + 25 * i;

		if (i == 0 || i == 11) {
			ILI9341_FillRectangle(display, x, 20, 1, 200, ILI9341_WHITE);
			continue;
		}

		for (uint16_t j = 0; j < 200; j += 2)
			ILI9341_DrawPixel(display, x, 20 + j, ILI9341_GRAY);
	}
}

static void clearCursor(ILI9341TypeDef *display, uint16_t pos)
{
	ILI9341_FillRectangle(display, 0, pos - 6, 20, 11, ILI9341_BLACK);
}

static void clearTrigger(ILI9341TypeDef *display, uint16_t pos)
{
	ILI9341_FillRectangle(display, 296, pos - 6, 20, 11, ILI9341_BLACK);
}

static void drawCursor(ILI9341TypeDef *display, uint16_t pos, char *name, uint16_t color)
{
	ILI9341_FillRectangle(display, 0, pos - 6, 7 * 2, 1, color);
	ILI9341_WriteString(display, 0, pos - 5, name, Font_7x10, ILI9341_BLACK, color);

	for (uint8_t i = 0; i < 6; i++) {
		for (uint8_t j = i; j < 11 - i; j++)
			ILI9341_DrawPixel(display, 14 + i, pos - 6 + j, color);
	}
}

static void drawTrigger(ILI9341TypeDef *display, uint16_t pos, char *name, uint16_t color)
{
	ILI9341_FillRectangle(display, 302, pos - 6, 7 * 2, 1, color);
	ILI9341_WriteString(display, 302, pos - 5, name, Font_7x10, ILI9341_BLACK, color);

	for (uint8_t i = 0; i < 6; i++) {
		for (uint8_t j = i; j < 11 - i; j++)
			ILI9341_DrawPixel(display, 301 - i, pos - 6 + j, color);
	}
}

static void drawSignal(ILI9341TypeDef *display, uint32_t *adc_time, uint16_t *adc0, uint32_t adc_length, uint16_t pixel_dirty[280][2], uint16_t cursor, uint16_t color)
{
	uint16_t point[280];
	for (uint16_t i = 0; i < 280; i++)
		point[i] = 0;

	for (uint16_t i = 0; i < adc_length; i++) {

		float uV = (float)(adc0[i]) * 3300000.0f / 4096.0f;
		uint16_t x = (float)(adc_time[i]) * 280.0f / (float)(12.0f * xlim_us);
		uint16_t y = cursor - ((uV / (float)(ylim_uV)) * 200.0f / 8.0f);

		if (x < 0)
			x = 0;

		if (x > 274)
			x = 274;

		if (y < 21)
			y = 21;

		if (y > 219)
			y = 219;

		point[x] += (float)(y - point[x]) * 1.0f;
	}

	uint16_t pixel[280][2];
	for (uint16_t i = 0; i < 280; i++) {
		pixel[i][0] = 220;
		pixel[i][1] = 20;
	}

	for (uint16_t i = 1; i <= 279; i++) {

		if (point[i] == 0)
			continue;

		int16_t x1 = i;
		int16_t x0 = x1 - 1;

		for (; x0 >= 0; x0--) {
			if (point[x0] != 0)
				break;
		}

		if (x0 == 0 && point[x0] == 0)
			return;

		int16_t y0 = point[x0];
		int16_t y1 = point[x1];

		int16_t dx = (x1 - x0) > 0 ? (x1 - x0) : -(x1 - x0);
		int16_t sx = x0 < x1 ? 1 : -1;
		int16_t dy = (y1 - y0) > 0 ? -(y1 - y0) : (y1 - y0);
		int16_t sy = y0 < y1 ? 1 : -1;
		int16_t error = dx + dy;

		while (1) {

			if (pixel[x0][0] > y0)
				pixel[x0][0] = y0;

			if (pixel[x0][1] < y0)
				pixel[x0][1] = y0;

			if (x0 == x1 && y0 == y1)
				break;

			int16_t e2 = 2 * error;

			if (e2 >= dy) {
				if (x0 == x1)
					break;

				error = error + dy;
				x0 = x0 + sx;
			}

			if (e2 <= dx) {
				if (y0 == y1)
					break;

				error = error + dx;
				y0 = y0 + sy;
			}
		}
	}

	for (uint16_t i = 1; i <= 279; i++) {
		uint16_t min = pixel[i][0] < pixel_dirty[i][0] ? pixel[i][0] : pixel_dirty[i][0];
		uint16_t max = pixel[i][1] > pixel_dirty[i][1] ? pixel[i][1] : pixel_dirty[i][1];

		for (uint16_t j = min; j <= max; j++) {
			uint8_t draw = 0;
			if (j >= pixel[i][0] && j <= pixel[i][1])
				draw = 1;

			uint8_t clear = 0;
			if (j >= pixel_dirty[i][0] && j <= pixel_dirty[i][1])
				clear = 1;

			if (draw && !clear && j > 21)
				ILI9341_DrawPixel(display, i + 20, j, color);

			if (!draw && clear) {
				if (((i % 25) == 0 && (j % 2) == 0) || ((i % 2) == 0 && ((j - 20) % 25) == 0))
					ILI9341_DrawPixel(display, i + 20, j,  ILI9341_GRAY);
				else
					ILI9341_DrawPixel(display, i + 20, j, ILI9341_BLACK);
			}
		}

		pixel_dirty[i][0] = pixel[i][0];
		pixel_dirty[i][1] = pixel[i][1];
	}
}

static void clearSignal(ILI9341TypeDef *display, uint16_t pixel_dirty[280][2])
{
	for (uint16_t i = 1; i <= 279; i++) {
		for (uint16_t j = pixel_dirty[i][0]; j <= pixel_dirty[i][1]; j++) {
			if (((i % 25) == 0 && (j % 2) == 0) || ((i % 2) == 0 && ((j - 20) % 25) == 0))
				ILI9341_DrawPixel(display, i + 20, j,  ILI9341_GRAY);
			else
				ILI9341_DrawPixel(display, i + 20, j, ILI9341_BLACK);
		}

		pixel_dirty[i][0] = 0;
		pixel_dirty[i][1] = 0;
	}
}

static void drawSignalParam(ILI9341TypeDef *display, char *string, size_t size, uint16_t adc_max, uint16_t adc_min, uint32_t adc_period)
{
	float max  = (float)(adc_max) * 3300000.0f / 4096.0f;
	float min  = (float)(adc_min) * 3300000.0f / 4096.0f;
	float freq = 1.0f / ((float)(adc_period) / 1000000.0f);
	char *max_postfix  = "";
	char *min_postfix  = "";
	char *freq_postfix = "";

	if (max >= 1000000.0f) {
		max /= 1000000.0f;
		max_postfix = " V";
	} else if (max >= 1000.0f) {
		max /= 1000.0f;
		max_postfix = "mV";
	} else
		max_postfix = "uV";

	if (min >= 1000000.0f) {
		min /= 1000000.0f;
		min_postfix = " V";
	} else if (min >= 1000.0f) {
		min /= 1000.0f;
		min_postfix = "mV";
	} else
		min_postfix = "uV";

	if (freq >= 1000000.0f) {
		freq /= 1000000.0f;
		freq_postfix = "MHz";
	} else if (freq >= 1000.0f) {
		freq /= 1000.0f;
		freq_postfix = "kHz";
	} else
		freq_postfix = " Hz";

	if (adc_period != 0)
		snprintf(string, size, "ampl:%3.1f%s~%3.1f%s freq:%3.0f%s  ", min, min_postfix, max, max_postfix, freq, freq_postfix);
	else
		snprintf(string, size, "ampl:%3.1f%s~%3.1f%s freq:??? Hz  ", min, min_postfix, max, max_postfix);

	ILI9341_WriteString(display, 98, 225, string, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
}

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
	  ILI9341_WriteString(&display, 0, 0, "Critical error!", Font_11x18, ILI9341_BLACK, ILI9341_RED);
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
