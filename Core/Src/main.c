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
#include "crc.h"
#include "dma.h"
#include "i2s.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
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
#include "easy_key.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "bmp280.h"
#include "fftaffect.h"
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
int fputc(int ch, FILE* f)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

uint16_t grambuff[1024];
uint16_t grambuff_usb[1024];
nex_usb_des des = {
.brides = {
	.brightness = 999
}
};
lv_anim_t anim_backlight;
void set_brightness_value(void *obj, int32_t value)
{
	// dbmsg("brightness: %d", value);
	Set_PWM_DutyCycle(value%1000);
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
}
float pitch, roll, yaw;

#define NPT 256									//样本数量
uint32_t i2s_dma[NPT*4];
int32_t fft_buf[NPT];
int32_t fft_raw[NPT];
int32_t fft_app[NPT];

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
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM13_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  MX_TIM3_Init();
  MX_I2S3_Init();
  /* USER CODE BEGIN 2 */
  dbmsg("system initialized");
	set_brightness_value(NULL, 0);
  USBD_Init(&hUSB, &FS_Desc, DEVICE_HS);
  USBD_RegisterClass(&hUSB, &USBD_NEX_LINK);
  USBD_NEX_LINK_Init(&hUSB, grambuff_usb, &des);
  USBD_Start(&hUSB);
  HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim3);
	MenuInit();
	EasyUIInit(1);
	dbmsg("MPU_Init = %d", MPU_Init());
//	dbmsg("mpu_dmp_init = %d\r\n", mpu_dmp_init());
	BMP280_Init();
	lv_anim_add(&anim_backlight, 0, set_brightness_value);
	lv_anim_start(&anim_backlight, des.brides.brightness, 2000);
	HAL_I2S_Receive_DMA(&hi2s3,(uint16_t*)i2s_dma,NPT*4);	
  dbmsg("application initialized");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
		lv_anim_run();
    HAL_Delay(1);
//		EasyUIEvent(5);
//		MPU_CRL(10);
//		BMP280_Test(1000);
		
    EasyUIClearBuffer();
		Display_Style1(fft_app);
		EasyUISendBuffer();
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
void GetPowerMag(int32_t *fftraw, int32_t *fft_app)
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
	
    for(i=0; i<NPT; i++)
    {
        lX  = (fftraw[i] << 16) >> 16;
        lY  = (fftraw[i] >> 16);
			
				//除以32768再乘65536是为了符合浮点数计算规律
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y)*1.0f/ NPT;
//				printf("%ld",(unsigned long)(Mag * 65536 / 16));
//				printf(",");
			fft_app[i] = Mag * 65536 / 16;
    }
//			printf("\r\n");
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s==&hi2s3){
		for(int i=0;i<NPT;i++)
		{
			//dat32 example: 0000fffb 00004f00
			fft_buf[i]=(i2s_dma[0+i*4]<<8)+(i2s_dma[1+i*4]>>8);
			
			if(fft_buf[i] & 0x800000){//negative
				fft_buf[i]|=0xff000000;
			}
			//printf("1:%08X,2:%08X,3:%08X,4:%08X,mix:%08X\n",i2s_dma[0+i*4],i2s_dma[1+i*4],i2s_dma[2+i*4],i2s_dma[3+i*4],fft_buf[i]);
			printf("%d\n",fft_buf[i]);
		}
//		HAL_I2S_Receive_DMA(&hi2s3,(uint16_t*)i2s_dma,NPT*4);	
		//256点FFT变换
//		cr4_fft_256_stm32(fft_raw, fft_buf, NPT);
//		GetPowerMag(fft_raw, fft_app);
		
	}
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
