/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MFRC522.h"
#include "LCD.h"
#include "RTC.h"
#include "string.h"
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
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, unsigned char *buf, int len)
{
	if(fd == 1 || fd == 2)
	{
		HAL_UART_Transmit(&huart2, buf, len, 999);
	}

	return len;
}

uint8_t uid_buffer[5], User1_Flag = 0, User2_Flag = 0;

MFRC522_t rfid = {&hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin, SPI1_RST_GPIO_Port, SPI1_RST_Pin};

char Scan_Card[] = "Scan Card", User1[] = "Shrvan", User2[] = "Yashraj",
     User1_TAG[] = "A1 1D 2C 1F", User2_TAG[] = "03 3C 6F 21",
	 Check_In[] = "Check-In",Check_Out[] = "Check-Out";

char TX_BUFFER[128];

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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  MFRC522_Init(&rfid);

  LCD_INIT();
  LCD_COMMAND(0x01);
  LCD_COMMAND(0x80);
  LCD_STRING(Scan_Card);

  setvbuf(stdout, NULL, _IONBF, 0);

  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x2345)
  {
	  set_time(16,03,00);
	  set_date(26,05,07,4);
  }

  set_time(18,11,00);
  set_date(26,05,9,6);

  char timeData[20];
  char dateData[20];

  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(WaitCardDetect(&rfid) == STATUS_OK)
	  {
		  if(MFRC522_Read_Uid(&rfid,uid_buffer) ==  STATUS_OK)
		  {
			  USER_LOG("CARD_ID:%02X %02X %02X %02X", uid_buffer[0], uid_buffer[1], uid_buffer[2], uid_buffer[3]);

		      if((uid_buffer[0] == 0xA1) && (uid_buffer[1] == 0x1D) && (uid_buffer[2] == 0x2C) && (uid_buffer[3] == 0x1F))
		      {
		    	  get_time_date(timeData,dateData);

		    	  if(User1_Flag == 0)
		    	  {

		    		  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);

			    	  snprintf(TX_BUFFER,sizeof(TX_BUFFER),"%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n", User1, User1_TAG, dateData, timeData, Check_In);

			    	  HAL_UART_Transmit_IT (&huart1, (uint8_t *) TX_BUFFER, strlen(TX_BUFFER));

			    	  //HAL_UART_Transmit(&huart1,(uint8_t *)TX_BUFFER, strlen(TX_BUFFER),100);

			    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);


			    	  LCD_COMMAND(0x01);
			    	  LCD_COMMAND(0x80);
			    	  LCD_STRING(User1);


			    	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(Check_In);
			    	  HAL_Delay(4000);


			    	  LCD_COMMAND(0x01);
			       	  LCD_COMMAND(0x80);
			    	  LCD_STRING(dateData);

			       	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(timeData);
			    	  HAL_Delay(2000);

			    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
			    	  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);

			    	  LCD_COMMAND(0x01);
			    	  LCD_STRING(Scan_Card);

			    	  User1_Flag = 1;
		    	  }

		    	  else
		    	  {
		    		  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);

			    	  snprintf(TX_BUFFER, sizeof(TX_BUFFER), "%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n", User1, User1_TAG, dateData, timeData, Check_Out);

			    	  HAL_UART_Transmit_IT (&huart1, (uint8_t *) TX_BUFFER, strlen(TX_BUFFER));

			    	  //HAL_UART_Transmit(&huart1,(uint8_t *)TX_BUFFER, strlen(TX_BUFFER),100);

			    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);


			    	  LCD_COMMAND(0x01);
			    	  LCD_COMMAND(0x80);
			    	  LCD_STRING(User1);

			    	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(Check_Out);
			    	  HAL_Delay(4000);


			    	  LCD_COMMAND(0x01);
			       	  LCD_COMMAND(0x80);
			    	  LCD_STRING(dateData);

			       	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(timeData);
			    	  HAL_Delay(2000);

			    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
			    	  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);

			    	  LCD_COMMAND(0x01);
			    	  LCD_STRING(Scan_Card);
			    	  User1_Flag = 0;		    	  }
		      }


		      else if((uid_buffer[0] == 0x03) && (uid_buffer[1] == 0x3C) && (uid_buffer[2] == 0x6F) && (uid_buffer[3] == 0x21))
		      {
		    	  get_time_date(timeData,dateData);

		    	  if(User2_Flag == 0)
		    	  {
		    		  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);

			    	  snprintf(TX_BUFFER,sizeof(TX_BUFFER),"%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n", User2, User2_TAG, dateData, timeData, Check_In);

			    	  HAL_UART_Transmit_IT (&huart1, (uint8_t *) TX_BUFFER, strlen(TX_BUFFER));

			    	  //HAL_UART_Transmit(&huart1,(uint8_t *)TX_BUFFER, strlen(TX_BUFFER),100);


			      	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

			    	  LCD_COMMAND(0x01);
			    	  LCD_COMMAND(0x80);
			    	  LCD_STRING(User2);

			    	  LCD_COMMAND(0xC0);
			       	  LCD_STRING(Check_In);
			     	  HAL_Delay(4000);


			    	  LCD_COMMAND(0x01);
			       	  LCD_COMMAND(0x80);
			    	  LCD_STRING(dateData);

			       	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(timeData);
			    	  HAL_Delay(2000);

			       	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
			       	  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);

			        	LCD_COMMAND(0x01);
			       	  LCD_STRING(Scan_Card);
			       	User2_Flag = 1;
		    	  }

		    	  else
		    	  {
		    		  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_SET);

			    	  snprintf(TX_BUFFER,sizeof(TX_BUFFER),"%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n", User2, User2_TAG, dateData, timeData, Check_Out);

			    	  HAL_UART_Transmit_IT (&huart1, (uint8_t *) TX_BUFFER, strlen(TX_BUFFER));

			    	  //HAL_UART_Transmit(&huart1,(uint8_t *)TX_BUFFER, strlen(TX_BUFFER),100);


			      	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

			    	  LCD_COMMAND(0x01);
			    	  LCD_COMMAND(0x80);
			    	  LCD_STRING(User2);

			    	  LCD_COMMAND(0xC0);
			       	  LCD_STRING(Check_Out);
			     	  HAL_Delay(4000);


			    	  LCD_COMMAND(0x01);
			       	  LCD_COMMAND(0x80);
			    	  LCD_STRING(dateData);

			       	  LCD_COMMAND(0xC0);
			    	  LCD_STRING(timeData);
			    	  HAL_Delay(2000);

			       	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
			       	  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);

			       	LCD_COMMAND(0x01);
			       	  LCD_STRING(Scan_Card);
			          User2_Flag = 0;
		    	  }
		      }

		      else
		      {

		    	  HAL_UART_Transmit_IT (&huart1, (uint8_t *) "ACCESS DENIED", 13);
		    	  LCD_COMMAND(0x01);
		    	  LCD_COMMAND(0x80);

		    	  LCD_STRING("ACCESS DENIED");
		    	  HAL_Delay(4000);

		    	  LCD_COMMAND(0x01);
		    	  LCD_STRING(Scan_Card);
		      }
		  }
		  WaitCardRemoval(&rfid);
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, D0_PIN_Pin|D1_PIN_Pin|D2_PIN_Pin|D3_PIN_Pin
                          |D4_PIN_Pin|D5_PIN_Pin|D6_PIN_Pin|D7_PIN_Pin
                          |RS_PIN_Pin|EN_PIN_Pin|LED1_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_PIN_GPIO_Port, BUZZER_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI1_RST_Pin|SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D0_PIN_Pin D1_PIN_Pin D2_PIN_Pin D3_PIN_Pin
                           D4_PIN_Pin D5_PIN_Pin D6_PIN_Pin D7_PIN_Pin
                           RS_PIN_Pin EN_PIN_Pin LED1_Pin LED2_Pin */
  GPIO_InitStruct.Pin = D0_PIN_Pin|D1_PIN_Pin|D2_PIN_Pin|D3_PIN_Pin
                          |D4_PIN_Pin|D5_PIN_Pin|D6_PIN_Pin|D7_PIN_Pin
                          |RS_PIN_Pin|EN_PIN_Pin|LED1_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_PIN_Pin */
  GPIO_InitStruct.Pin = BUZZER_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_PIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_RST_Pin SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_RST_Pin|SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
