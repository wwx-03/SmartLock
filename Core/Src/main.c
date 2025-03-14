/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "SystemFuction.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
lcd_handle_t lcd_1;
key_handle_t key_1 = {
	.IO = {
		.C1 = {GPIOC, GPIO_PIN_6},
		.C2 = {GPIOC, GPIO_PIN_7},
		.C3 = {GPIOC, GPIO_PIN_8},
		.C4 = {GPIOC, GPIO_PIN_9},
		.R1 = {GPIOB, GPIO_PIN_12},
		.R2 = {GPIOB, GPIO_PIN_13},
		.R3 = {GPIOB, GPIO_PIN_14},
		.R4 = {GPIOB, GPIO_PIN_15},
	},
};
as608_handle_t as608_1;
rc522_handle_t rc522_1 = {
  .IO = {
    .SDA = {GPIOB, GPIO_PIN_8},
    .SCK = {GPIOB, GPIO_PIN_7},
    .MOSI = {GPIOB, GPIO_PIN_6},
    .MISO = {GPIOB, GPIO_PIN_9},
    .RST = {GPIOC, GPIO_PIN_13},
    .IRQ = {GPIOC, GPIO_PIN_14},
  }
};
esp_handle_t esp_1 = {
  .huartx = &huart3,
  .rxBufferAddr = rx3Buffer,
  .rxBufferSize = 256,
  .txBufferAddr = tx3Buffer,
  .txBufferSize = 256,
  .io.rst = {GPIOB, GPIO_PIN_1},
};
password_structure_t sys_password = {
	.len = 6
};
password_structure_t password;


sccb_handle_t sccb_1 = 
{
  .id_addr = 0x60,
  .io = 
  {
    .scl = {GPIOC, GPIO_PIN_0},
    .sda = {GPIOC, GPIO_PIN_2},
  },
};

ov2640_handle_t ov2640_1 = 
{
  .io = 
  {
    .data = 
    {
      {GPIOA, GPIO_PIN_0},
      {GPIOA, GPIO_PIN_1},
      {GPIOA, GPIO_PIN_2},
      {GPIOA, GPIO_PIN_3},
      {GPIOA, GPIO_PIN_4},
      {GPIOA, GPIO_PIN_5},
      {GPIOA, GPIO_PIN_6},
      {GPIOA, GPIO_PIN_7},
    },
    .dclk = {GPIOC, GPIO_PIN_4},
    .pwdn = {GPIOC, GPIO_PIN_5},
    .href = {GPIOC, GPIO_PIN_1},
    .rst = {GPIOC, GPIO_PIN_3},
    .vsync = {GPIOC, GPIO_PIN_15},
  },
  .sccb_handle = &sccb_1,
};

onenet_handle_t onenet_1 = 
{
  .esp = &esp_1,
};

motor_handle_t motor_1 = 
{
  .IO = 
  {
    .MD0 = {GPIOD, GPIO_PIN_2},
    .MD1 = {GPIOC, GPIO_PIN_12},
    .MD2 = {GPIOC, GPIO_PIN_11},
    .MD3 = {GPIOC, GPIO_PIN_10},
  },
};

system_handle_t sys = 
{
	.as608 = &as608_1,
	.key = &key_1,
	.lcd = &lcd_1,
	.password = &sys_password,
  .rc522 = &rc522_1,
  .esp = &esp_1,
  .ov2640 = &ov2640_1,
  .onenet = &onenet_1,
  .motor = &motor_1,
};

uint16_t keynum;
uint8_t finger_flag;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Task_KeyScan(system_handle_t *system);
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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	System_Init(&sys, 160 / 16 - 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    System_UpdateLockStatus(&sys);
    System_UpdataMotor(&sys);
    ESP_Run(&esp_1);
    OneNet_Run(&onenet_1);
    if(sys.errorLock == 0)
    {
      Task_KeyScan(&sys);
    }
    else
    {
      System_ShowRestErrorMillis(&sys);
    }

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/* USER CODE BEGIN 4 */
void Task_KeyScan(system_handle_t *system)
{
	uint8_t keynum = 0, fingerFlag = 0;
  uint8_t pageContent = system->pageContent, content = system->content;
	keynum = Key_GetNum(system->key);
  fingerFlag = AS608_GetWAK(system->as608);
	if(keynum != 0 && keynum <= 10)
	{
		System_UnlockByPassword(system);
    System_ShowFunction(system);
	}
	else if(keynum == 14)
	{
		if(system->index > 0)
		{
			system->index --;
		}
		else
		{
			system->index = content - 1;
		}
    System_ShowFunction(system);
	}
	else if(keynum == 15)
	{
		if(system->index < content - 1)
		{
			system->index ++;
		}
		else
		{
			system->index = 0;
		}
    System_ShowFunction(system);
	}
	else if(keynum == 16)
	{
		System_DoFunction(system);
    System_ShowFunction(system);
	}
	else if(fingerFlag == 1)
	{
		System_UnlockByFingerprint(system);
    System_ShowFunction(system);
	}
  else if(RC522_PcdRequest(system->rc522, PICC_REQALL, NULL) == MI_OK)
  {
    System_UnlockByIDCard(system);
    System_ShowFunction(system);
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
  while (1)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"Error\r\n", sizeof("Error\r\n"), HAL_MAX_DELAY);
    HAL_Delay(1000);
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
