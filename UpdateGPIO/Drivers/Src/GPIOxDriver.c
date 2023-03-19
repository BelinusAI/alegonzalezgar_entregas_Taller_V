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

	pinValue = (1 << 0) & (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	return pinValue;
}

/* Cambiar el estado de un PINx */
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
	// Recibe como parámetro un elemento del tipo GPIO_Handler_t
	// Cambia el estado del PIN: sí está encendido pasa a apagado, si está apagado pasa a encendido.

	uint32_t pinValue = GPIO_ReadPin(pPinHandler);

	if(SET ^ pinValue){ // PIN apagado
		GPIO_WritePin(pPinHandler, SET);
	}
	else{ // PIN encendido
		GPIO_WritePin(pPinHandler, RESET);
	}
}
