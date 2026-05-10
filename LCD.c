#include "LCD.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/*void LCD_GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = D0_PIN | D1_PIN | D2_PIN | D3_PIN | D4_PIN | D5_PIN | D6_PIN | D7_PIN | RS_PIN | EN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init ( LCD_PORT, &GPIO_InitStruct);
}*/


void LCD_INIT()
{
	LCD_COMMAND(0x38);
	HAL_Delay(20);

	LCD_COMMAND(0x06);
    HAL_Delay(20);

	LCD_COMMAND(0x0C);
	HAL_Delay(20);

	LCD_COMMAND(0x01);
	HAL_Delay(20);
}

void LCD_COMMAND(unsigned char command)
{
	 HAL_GPIO_WritePin (GPIOC,RS_PIN_Pin ,GPIO_PIN_RESET);

	 HAL_GPIO_WritePin (GPIOC,D0_PIN_Pin ,(command & 0x01));
	 HAL_GPIO_WritePin (GPIOC,D1_PIN_Pin ,(command & 0x02)>>1);
	 HAL_GPIO_WritePin (GPIOC,D2_PIN_Pin ,(command & 0x04)>>2);
	 HAL_GPIO_WritePin (GPIOC,D3_PIN_Pin ,(command & 0x08)>>3);
	 HAL_GPIO_WritePin (GPIOC,D4_PIN_Pin ,(command & 0x10)>>4);
	 HAL_GPIO_WritePin (GPIOC,D5_PIN_Pin ,(command & 0x20)>>5);
	 HAL_GPIO_WritePin (GPIOC,D6_PIN_Pin ,(command & 0x40)>>6);
	 HAL_GPIO_WritePin (GPIOC,D7_PIN_Pin ,(command & 0x80)>>7);

	 HAL_GPIO_WritePin (GPIOC,EN_PIN_Pin ,GPIO_PIN_SET);
	 HAL_Delay(5);
	 HAL_GPIO_WritePin (GPIOC,EN_PIN_Pin ,GPIO_PIN_RESET);
	 HAL_Delay(2);
}


void LCD_DATA(unsigned char data)
{
	     HAL_GPIO_WritePin (GPIOC,RS_PIN_Pin ,GPIO_PIN_SET);


		 HAL_GPIO_WritePin (GPIOC,D0_PIN_Pin ,(data & 0x01));
		 HAL_GPIO_WritePin (GPIOC,D1_PIN_Pin ,(data & 0x02)>>1);
		 HAL_GPIO_WritePin (GPIOC,D2_PIN_Pin ,(data & 0x04)>>2);
		 HAL_GPIO_WritePin (GPIOC,D3_PIN_Pin ,(data & 0x08)>>3);
		 HAL_GPIO_WritePin (GPIOC,D4_PIN_Pin ,(data & 0x10)>>4);
		 HAL_GPIO_WritePin (GPIOC,D5_PIN_Pin ,(data & 0x20)>>5);
		 HAL_GPIO_WritePin (GPIOC,D6_PIN_Pin ,(data & 0x40)>>6);
		 HAL_GPIO_WritePin (GPIOC,D7_PIN_Pin ,(data & 0x80)>>7);

		 HAL_GPIO_WritePin (GPIOC,EN_PIN_Pin ,GPIO_PIN_SET);
		 HAL_Delay(5);
		 HAL_GPIO_WritePin (GPIOC,EN_PIN_Pin ,GPIO_PIN_RESET);
		 HAL_Delay(2);

}

void LCD_STRING(char *str)
{
	while(*str != '\0')
	{
		LCD_DATA(*str++);
		HAL_Delay(10);
	}
}
