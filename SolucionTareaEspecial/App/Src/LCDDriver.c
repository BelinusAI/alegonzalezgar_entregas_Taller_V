#include <stdint.h>
#include <string.h>
#include "LCDDriver.h"

// Para dividir el número de casillas en dos de 4 bits
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd){
	char _U; // Para mandar 4 bits de arriba
	char _L; // Para mandar los 4 bits de abajo
	uint8_t _T[4];
	_U=(cmd & 0xf0);
	_L=((cmd<<4) & 0xf0);
	_T[0] = _U|0x0C;
	LCD_writeData(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x09;
	LCD_writeData(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0C;
	LCD_writeData(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x09;
	LCD_writeData(ptrHandlerI2C, _T[3]);
}

// Enviar dato I2C
void LCD_writeData(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){

	/* 1. Generamos la condición de Start*/
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Enviamos la diracción del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	i2c_sendDataByte(ptrHandlerI2C, dataToWrite);

	i2c_stopTransaction(ptrHandlerI2C);
}

// Para los datos que se van a enviar a la pantalla
void LCD_sendata (I2C_Handler_t *ptrHandlerI2C, char data){
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(data & 0xf0);
	_L=((data<<4) & 0xf0);
	_T[0] = _U|0x0D;
	LCD_writeData(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x09;
	LCD_writeData(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0D;
	LCD_writeData(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x09;
	LCD_writeData(ptrHandlerI2C, _T[3]);
}

// Para limpiar toda la pantalla
void LCD_Clear (I2C_Handler_t *ptrHandlerI2C) {
	for(int i = 0; i < 80; i++){
		LCD_sendSTR(ptrHandlerI2C, " ");
	}

}

void LCD_Init (I2C_Handler_t *ptrHandlerI2C) {
	SysTick_Config(0);
	// Hitachi LCD datasheet pp.46
	// Tiempo de refresco de la pantalla
	delay_ms(50);
	// Para inicializar la interface de 4 bits hacemos lo siguiente
	// Primero 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay_ms(5);
	// Segundo 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay_ms(2);
	// Tercero 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);

	// Delay después de la secuencia inicial de inicializacion
	delay_ms(50);

	/*
	 * Configuraciones para la escritura
	 */
	// Data lenght 4, lines 2, character font 5X8
	LCD_sendCMD (ptrHandlerI2C, 0x20);
	delay_ms(50);
	LCD_sendCMD (ptrHandlerI2C, 0x28);
	delay_ms(50);
	// Display off
	LCD_sendCMD (ptrHandlerI2C, 0x08);
	delay_ms(50);
	// Display Clear
	LCD_sendCMD (ptrHandlerI2C, 0x01);
	delay_ms(50);
	// Entry mode
	LCD_sendCMD (ptrHandlerI2C, 0x06);

	delay_ms(50);
	// Delay para encendido
}

void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str) {
	while (*str) LCD_sendata(ptrHandlerI2C, *str++);
}

void shift(I2C_Handler_t *ptrHandlerI2C, int n){
	for(int i = 1; i < n; i++){
		LCD_sendCMD(ptrHandlerI2C, 0b0000011100);
	}

}



