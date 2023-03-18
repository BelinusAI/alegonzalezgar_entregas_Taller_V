/*
 * GPIOxDriver.c
 *
 *  Created on: 16/03/2023
 *      Author: alegonzalezgar
 */

/******
 * En este archivo se escribe el control para que sea lo mas generico
 * posble, de forma que independiente del puerto GPIO y el PIN seleccionado,
 * el programa se ejecute y configure todo correctamente
 ******/

#include "GPIOxDriver.h"

/* Valores estándar para las configuraciones */
/* 8.4.1 GPIOx_MODER */
#define GPIO_MODE_IN			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALTFN			2
#define GPIO_MODE_ANALOG		3

/* 8.4.2 GPIOx_OTYPER */
#define GPIO_OTYPE_PUSHPULL		0
#define GPIO_OTYPE_OPENDRAIN	1

/* 8.4.3 GPIOx_OSPEEDR */
#define GPIO_OSPEEDR_LOW		0
#define GPIO_OSPEEDR_MEDIUM		1
#define GPIO_OSPEEDR_FAST		2
#define GPIO_OSPEEDR_HIGH		3

/* 8.4.4 GPIOx_PUPDR */
#define GPIO_PUPDR_NOTHING		0
#define GPIO_PUPDR_PULLUP		1
#define GPIO_PUPDR_PULLDOWN		2
#define GPIO_PUPDR_RESERVED		3

/* Definición de los nombres de los pines */
#define PIN_0			0
#define PIN_1			1
#define PIN_2			2
#define PIN_3			3
#define PIN_4			4
#define PIN_5			5
#define PIN_6			6
#define PIN_7			7
#define PIN_8			8
#define PIN_9			9
#define PIN_10			10
#define PIN_11			11
#define PIN_12			12
#define PIN_13			13
#define PIN_14			14
#define PIN_15			15

/* Definición de las funciones alternativas */
#define AF0			0b0000
#define AF1			0b0001
#define AF2			0b0010
#define AF3			0b0011
#define AF4			0b0100
#define AF5			0b0101
#define AF6			0b0110
#define AF7			0b0111
#define AF8			0b1000
#define AF9			0b1001
#define AF10		0b1010
#define AF11		0b1011
#define AF12		0b1100
#define AF13		0b1101
#define AF14		0b1110
#define AF15		0b1111

/*
 * Para cualquier Periferico, hay ciertos paso para que el sistema deje configurarlo
 * Lo primero y màs importante es activar la señal de reloj principal hacia ese elemento
 * especifico (relacionado con el periferico RCC)
 */
void GPIO_Config(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//Activar el periférico
	if(pGPIOHandler->pGPIOx==GPIOA){
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);

	}
	else if(pGPIOHandler->pGPIOx==GPIOB){
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);

	}
	else if(pGPIOHandler->pGPIOx==GPIOC){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);

	}
	else if(pGPIOHandler->pGPIOx==GPIOD){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);

	}
	else if(pGPIOHandler->pGPIOx==GPIOE){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);

	}
	else if(pGPIOHandler->pGPIOx==GPIOH){
			RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);

	}

	//Configurar registro GPIOx_Moder
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->MODER |= auxConfig;

	//Configurar GPIOx_OTYPER
	auxConfig = (pGPIOHandler->GPIO_PinConfig.PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler ->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	//Configurar velocidad OSPEEDR
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

	//Configurar pull-up,pull-up, flotante PUPDR
	auxConfig = (pGPIOHandler->GPIO_PinConfig.PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

	//Configurar funciones alternativas
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		//Registro AFRL
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			pGPIOHandler->pGPIOx->AFR[0] &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);
		}
		else{
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber - 8);

			pGPIOHandler->pGPIOx->AFR[1] &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);

		}
	}

} //Fin GPIO Config

//Cambiar el estado del pin, asignando el valor entregado en newState
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){
	//pPinHandler ->pGPIOx->ODR &= ~(SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	if(newState == SET){
		//Parte baja del registro
		pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber));
	}
	else{
		//Parte alta del registro
		pPinHandler->pGPIOx->BSRR |=(SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}

/*Leer el estado de un Pin*/
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){
	uint32_t pinValue = 0;

	pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	return pinValue;
}
