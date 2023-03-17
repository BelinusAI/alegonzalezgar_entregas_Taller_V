/*
 * GPIOxDriver.c
 *
 *  Created on: 16/03/2023
 *      Author: alegonzalezgar
 *
 * Control
 */

#include "GPIOxDriver.h"


void GPIO_Config(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//Activar el periférico
	if(pGPIOHandler->pGPIOx==GPIOA){
		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOA_EN);
	}
	else if(pGPIOHandler->pGPIOx==GPIOB){
		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOB_EN);
	}
	else if(pGPIOHandler->pGPIOx==GPIOC){
			RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOC_EN);
	}
	else if(pGPIOHandler->pGPIOx==GPIOD){
			RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOD_EN);
	}
	else if(pGPIOHandler->pGPIOx==GPIOE){
			RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOE_EN);
	}
	else if(pGPIOHandler->pGPIOx==GPIOH){
			RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOH_EN);
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

			pGPIOHandler->pGPIOx->AFRL &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFRL |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);
		}
		else{
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber - 8);

			pGPIOHandler->pGPIOx->AFRH &= ~(0b1111 << auxPosition);
			pGPIOHandler->pGPIOx->AFRH |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);

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
