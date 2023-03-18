/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : main
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>

#include "stm32f411_hal.h"
#include "GPIOxDriver.h"

/**
 * Función principal del programa
*/
int main(void){

	/* Pregunta a resolver 1)
	 * a)La función GPIO_ReadPin() debe obtener el valor PinX, para ello utiliza la variable PinValue que es
	 * inicializada con valor 0; el valor de PinX se encuentra en la posición de memoria GPIO_PinNumber;la
	 * declaración pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber desplaza el valor de PinX
	 * a la posición 0. Cuando el valor obtenido se asigana directamente a PinValue, se está obteniendo el valor de
	 * IDR y no solo el valor del Pin(x); b) para obtener el valor PinX se debe a PinX la mascara (1 << 0) y se obtiene
	 * el y-lógico entre ambos valores.
	 */


    ///Definir Handler para el PIN
	GPIO_Handler_t handlerUserLedPinPA7 = {0};
	GPIO_Handler_t handlerUserLedPinPC8 = {0};
	GPIO_Handler_t handlerUserLedPinPC7 = {0};
	GPIO_Handler_t handlerUserLedPinPA6 = {0};
	GPIO_Handler_t handlerUserLedPinPB8 = {0};
	GPIO_Handler_t handlerUserLedPinPC6 = {0};
	GPIO_Handler_t handlerUserLedPinPC9 = {0};
	GPIO_Handler_t handlerUserLedPinPA5 = {0};
	GPIO_Handler_t handlerUserLedPinPC13 = {0};

	/*Configurar puertos*/
	//Puerto PA7
	handlerUserLedPinPA7.pGPIOx = GPIOA;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA7.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA7.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPA7.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC8
	handlerUserLedPinPC8.pGPIOx = GPIOC;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC8.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPC8.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC7
	handlerUserLedPinPC7.pGPIOx = GPIOC;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC7.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC7.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPC7.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PA6
	handlerUserLedPinPA6.pGPIOx = GPIOA;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPA6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PB8
	handlerUserLedPinPB8.pGPIOx = GPIOB;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPB8.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPB8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPB8.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC6
	handlerUserLedPinPC6.pGPIOx = GPIOC;
	handlerUserLedPinPC6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserLedPinPC6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPC6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC9
	handlerUserLedPinPC9.pGPIOx = GPIOC;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC9.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC9.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPC9.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PA5 LED2
	handlerUserLedPinPA5.pGPIOx = GPIOA;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA5.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA5.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPA5.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC13 B1 USER Buttom
	handlerUserLedPinPC13.pGPIOx = GPIOC;
	handlerUserLedPinPC13.GPIO_PinConfig.GPIO_PinNumber	= PIN_13;
	handlerUserLedPinPC13.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserLedPinPC13.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC13.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerUserLedPinPC13.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_MEDIUM;
	handlerUserLedPinPC13.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	/*Cargar configuración*/
	GPIO_Config(&handlerUserLedPinPA7);
	GPIO_Config(&handlerUserLedPinPC8);
	GPIO_Config(&handlerUserLedPinPC7);
	GPIO_Config(&handlerUserLedPinPA6);
	GPIO_Config(&handlerUserLedPinPB8);
	GPIO_Config(&handlerUserLedPinPC6);
	GPIO_Config(&handlerUserLedPinPC9);
	GPIO_Config(&handlerUserLedPinPA5);
	GPIO_Config(&handlerUserLedPinPC13);

	/*
	 * Los LEDs están organizados  de forma que representen en número binario de 7 bits el "contador"
	 *  en el orden que se presenta a continuación:
	 * bit-6	bit-5	bit-4	bit-3	bit-2	bit-1	bit-0
	 * PC9		PC6		PB8		PA6		PC7		PC8		PA7
	 */
	uint8_t counter = 0b0000001; // Contador inicia en 1

	while(1){

		//Encender los LEDS que representan a "contador" en binario
		GPIO_WritePin(&handlerUserLedPinPA7, ((1 << 0) & counter) >> 0);	//bit-0
		GPIO_WritePin(&handlerUserLedPinPC8, ((1 << 1) & counter) >> 1);	//bit-1
		GPIO_WritePin(&handlerUserLedPinPC7, ((1 << 2) & counter) >> 2);	//bit-2
		GPIO_WritePin(&handlerUserLedPinPA6, ((1 << 3) & counter) >> 3);	//bit-3
		GPIO_WritePin(&handlerUserLedPinPB8, ((1 << 4) & counter) >> 4);	//bit-4
		GPIO_WritePin(&handlerUserLedPinPC6, ((1 << 5) & counter) >> 5);	//bit-5
		GPIO_WritePin(&handlerUserLedPinPC9, ((1 << 6) & counter) >> 6);	//bit-6

		for(uint32_t i = 0; i < 1350000; i++){}; //Delay Aprox 1 seg total

		if(GPIO_ReadPin(&handlerUserLedPinPC13)){ //USER NO presionado
			//LED2 encendido para indicar que USER No está presionado
			GPIO_WritePin(&handlerUserLedPinPA5, SET);

			counter++; //El contador incrementa su valor
			if(counter == 61) {counter = 1;} //Al llegar a 60 e contador reinicia en 1

		}
		else{ //USER presionado
			//LED2 apagado para indicar que USER está presionado
			GPIO_WritePin(&handlerUserLedPinPA5, CLEAR);

			counter--; //El contador decrementa su valor
			if(counter == 0){counter = 60;} //Al llegar a 1 e contador reinicia en 60

		}
	}
}
