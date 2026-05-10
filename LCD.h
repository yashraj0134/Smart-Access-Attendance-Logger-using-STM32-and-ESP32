/*
 * LCD.h
 *
 *  Created on: May 6, 2026
 *      Author: Yashraj
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
/*#define LCD_PORT GPIOC

#define D0_PIN GPIO_PIN_0
#define D1_PIN GPIO_PIN_1
#define D2_PIN GPIO_PIN_2
#define D3_PIN GPIO_PIN_3
#define D4_PIN GPIO_PIN_4
#define D5_PIN GPIO_PIN_5
#define D6_PIN GPIO_PIN_6
#define D7_PIN GPIO_PIN_7

#define RS_PIN GPIO_PIN_8
#define EN_PIN GPIO_PIN_9*/
//FUNCTIONS

//void LCD_GPIO_INIT(void);
void LCD_INIT(void);
void LCD_COMMAND(unsigned char command);
void LCD_DATA(unsigned char data);
void LCD_STRING(char *str);


#endif /* INC_LCD_H_ */
