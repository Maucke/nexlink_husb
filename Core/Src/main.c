/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2s.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdarg.h>
#include "usbd_def.h"
#include "usbd_desc.h"
#include "usbd_core.h"
#include "usbd_nex_link.h"
#include "nex_usb.h"
#include "stdio.h"
#include "queue.h"
#include "nv3030b.h"
#include "lv_anim_light.h"
#include "easy_ui.h"
#include "easy_ui_user_app.h"
#include "easy_key.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "bmp280.h"
#include "fftaffect.h"
#include "rx8900.h"
//#include "arm_math.h"
//#include "chipmunkdemo.h"
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

/* USER CODE BEGIN PV */
USBD_HandleTypeDef hUSB;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//int fputc(int ch, FILE* f)
//{
//  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
//  return ch;
//}

uint8_t debug_buf[DEBUG_BUF_SIZE] = {0};
extern bool usbavaliable;
int usb_printf(const char* pcFormat, ...)
{
  va_list args;
  int len = 0;
  memset(debug_buf, 0, sizeof debug_buf);
  va_start(args, pcFormat);

  len = vsnprintf((char*)debug_buf, sizeof(debug_buf), pcFormat, args);
	if(usbavaliable)
		USBD_NEX_LINK_Transmit(&hUSB, debug_buf, len);
  va_end(args);

  return len;
}

uint16_t grambuff[1024];
uint16_t grambuff_usb[1024];

nex_usb_des des = {
.brides = {
	.brightness = 299,
	.damp = 500
}
};
lv_anim_t anim_backlight;
void set_brightness_value(void *obj, int32_t value)
{
	// dbmsg("brightness: %d", value);
	Set_PWM_DutyCycle(value%1000);
}

void ready_brightness_value(struct _lv_anim_t *obj)
{
	dbmsg("brightness: %d", ((lv_anim_t*)obj)->end_value);
	if(((lv_anim_t*)obj)->end_value == 0)
	{
			dbusbmsg("system shutdown");
			HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_RESET);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim3.Instance)
	{
		EasyKeyScanKeyState();
		EasyKeyUserApp();
		EasyUIKeyActionMonitor();
//		dbmsg("tick: %d", HAL_GetTick());
	}
	if (htim->Instance == htim14.Instance)
	{
		lowBatteryAction();
	}
}

lv_anim_t anim_beep;
void set_beep_value(void *obj, int32_t value)
{
	Set_Freqeucy_Cycle(value);
}

void ready_beep_value(struct _lv_anim_t *obj)
{
	if(((lv_anim_t*)obj)->end_value != 0)
	{
			dbusbmsg("beep");
			lv_anim_start(&anim_beep, 0, 100);
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    __enable_irq();
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
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM13_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  MX_TIM3_Init();
  MX_I2S3_Init();
  MX_ADC1_Init();
  MX_TIM14_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
	MenuInit();
  dbmsg("system initialized");

	set_brightness_value(NULL, 0);
  USBD_Init(&hUSB, &FS_Desc, DEVICE_HS);
  USBD_RegisterClass(&hUSB, &USBD_NEX_LINK);
  USBD_NEX_LINK_Init(&hUSB, grambuff_usb, &des);
  USBD_Start(&hUSB);
  HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim14);
	RX8900_Init();
	EasyUIInit(1);
	dbmsg("MPU_Init = %d", MPU_Init());
//	dbmsg("mpu_init = %d", mpu_init());
	BMP280_Init();
	lv_anim_add(&anim_backlight, 0, set_brightness_value);
	lv_anim_ready_set_cb(&anim_backlight, ready_brightness_value);
	
	lv_anim_add(&anim_beep, 0, set_beep_value);
	lv_anim_path_set_cb(&anim_beep, lv_anim_path_onoff);
	lv_anim_ready_set_cb(&anim_beep, ready_beep_value);
  dbmsg("application initialized");
	lv_anim_start(&anim_backlight, des.brides.brightness, des.brides.damp);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		lv_anim_run();
		EasyUIEvent(5);
		EventJump();
		MPU_CRL(10);
//		BMP280_Test(1000);
//		RX8900_Test(1000);
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
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s==&hi2s3){
//		for (int i = 0;i < 256; i++)
//		{
// 			fft_input_buffer[i] =(adc_buffer[0+i*4]<<8)+(adc_buffer[1+i*4]>>8);
//			
//			if(fft_input_buffer[i] & 0x800000){//negative
//					fft_input_buffer[i]|=0xff000000;
//			}
//		}
	}
}
#define FLASH_ADDRESS 0x08000000 
typedef void (*pFunction)(void);
pFunction                     JumpAddress;

void JumpToApplication()
{
  JumpAddress = *(__IO pFunction*)(FLASH_ADDRESS + 4);
  __set_MSP(*(__IO uint32_t*) FLASH_ADDRESS);
  HAL_DeInit();
  JumpAddress();
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
	HAL_GPIO_WritePin(PW_HOLD_GPIO_Port, PW_HOLD_Pin, GPIO_PIN_RESET); //cut the power

  while(1)
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
