/*
 * GPIOxDriver.c
 *
 *  Created on: 16/03/2023
 *      Author: alegonzalezgar
 *
 * Control
 */

#include "GPIOxDriver.h"

/* Configurar un periférico */
void GPIO_Config(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	//Activar el periférico (reloj)
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

	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber); //clear
	pGPIOHandler->pGPIOx->MODER |= auxConfig;	//set

	//Configurar GPIOx_OTYPER
	auxConfig = (pGPIOHandler->GPIO_PinConfig.PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler ->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);	//clear
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;	//set

	//Configurar velocidad OSPEEDR
	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);	//clear
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;	//set

	//Configurar pull-up,pull-up, flotante PUPDR
	auxConfig = (pGPIOHandler->GPIO_PinConfig.PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);	//clear
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;	//set

	//Configurar funciones alternativas
	if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		//Registro bajo AFRL
		if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			pGPIOHandler->pGPIOx->AFRL &= ~(0b1111 << auxPosition);	//clear
			pGPIOHandler->pGPIOx->AFRL |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);	//set
		}
		//Registro alto AFRH
		else{
			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber - 8);

			pGPIOHandler->pGPIOx->AFRH &= ~(0b1111 << auxPosition);	//clear
			pGPIOHandler->pGPIOx->AFRH |= (pGPIOHandler->GPIO_PinConfig.PinAltFunMode << auxPosition);	//set

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

	//Pregunta a resolver 1 c) (Ver comentario en main.c)
	pinValue = (1 << 0) & (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	return pinValue;
}

//Pregunta a resolver 2)
/* Cambiar el estado de un PINx */
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
	// Recibe como parámetro un elemento del tipo GPIO_Handler_t
	// Cambia el estado del PIN: sí está encendido pasa a apagado, si está apagado pasa a encendido.

	uint32_t pinValue = GPIO_ReadPin(pPinHandler);

	if(SET ^ pinValue){ // PIN apagado
		GPIO_WritePin(pPinHandler, SET);
	}
	else{ // PIN encendido
		GPIO_WritePin(pPinHandler, CLEAR);
	}
}

