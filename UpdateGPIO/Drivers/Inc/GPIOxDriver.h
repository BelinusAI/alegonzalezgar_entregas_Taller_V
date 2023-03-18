/*
 * GPIOxDriver.h
 *
 *  Created on: 16/03/2023
 *      Author: alegonzalezgar
 *
 * Manejo y control del periférico GPIOx
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

#include <stm32f4xx.h>

//Configuración del pin
typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t PinPuPdControl;			//Salida Pull Up, Pull Down, "Libre"
	uint8_t PinOPType;				//Salida PUPD or OpenDrain
	uint8_t PinAltFunMode;			//Función alternativa

}GPIO_PinConfig_t;

typedef struct
{
	GPIO_TypeDef		*pGPIOx;
	GPIO_PinConfig_t	GPIO_PinConfig;

}GPIO_Handler_t;


/*Definición de las cabeceras de las funciones del GPIOxDriver */
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);



#endif /* GPIOXDRIVER_H_ */
