#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_


#define LCD_ADDRESS		0b0100110 //0100110


#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"

void LCD_Init(I2C_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str);
void LCD_setCursor (I2C_Handler_t *ptrHandlerI2C, uint8_t filas, uint8_t columnas);
void LCD_writeData(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
void LCD_Clear (I2C_Handler_t *ptrHandlerI2C);
void LCD_Clear_Screen(I2C_Handler_t *ptrHandlerI2C);


#endif /* LCDDRIVER_H_ */
