#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#define LCD_ADDRESS		0x23 //0100011

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
void shift(I2C_Handler_t *ptrHandlerI2C, int n);

#endif /* LCDDRIVER_H_ */
