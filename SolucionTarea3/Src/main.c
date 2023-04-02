/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


//Definición de elementos
GPIO_Handler_t handlerUserLedPinPA5 = {0};
GPIO_Handler_t handlerUserLedPinPC7 = {0};
GPIO_Handler_t handlerUserLedPinPC9 = {0};
GPIO_Handler_t handlerUserLedPinPA9 = {0};
GPIO_Handler_t handlerUserLedPinPA8 = {0};
GPIO_Handler_t handlerUserLedPinPB8 = {0};
GPIO_Handler_t handlerUserLedPinPB9 = {0};
GPIO_Handler_t handlerUserLedPinPA7 = {0};
GPIO_Handler_t handlerUserLedPinPC8 = {0};
GPIO_Handler_t handlerUserLedPinPB6 = {0};
GPIO_Handler_t handlerUserLedPinPA6 = {0};

GPIO_Handler_t handlerUserLedPinPC6 = {0};
GPIO_Handler_t handlerUserLedPinPC13 = {0};

BasicTimer_Handler_t handlerBlinkyTimer2 = {0};
BasicTimer_Handler_t handlerBlinkyTimer3 = {0};

EXTI_Config_t extiConfigPC13 = {0};

uint32_t counter_Exti13 = 0;

//Lista de numeros del uno al nueve para el 7 segmentos
uint16_t numero[10] = {0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011,
		0b01011011, 0b01011111, 0b01110000 , 0b01111111, 0b01111011};

//Prototipos de funciones
void init_Hadware(void);
void numeroDisplay(int);

int main(void){
	init_Hadware();

	while(1){

	}

	return 0;

}

void init_Hadware(void){
	//Puerto PA5 LED2
	handlerUserLedPinPA5.pGPIOx = GPIOA;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA5.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA5.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA5.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA5.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC7
	handlerUserLedPinPC7.pGPIOx = GPIOC;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC7.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC7.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPC7.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC9 DispUni
	handlerUserLedPinPC9.pGPIOx = GPIOC;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC9.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC9.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC9.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPC9.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PB8
	handlerUserLedPinPB8.pGPIOx = GPIOB;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPB8.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPB8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPB8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPB8.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PB9
	handlerUserLedPinPB9.pGPIOx = GPIOB;
	handlerUserLedPinPB9.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerUserLedPinPB9.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPB9.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPB9.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPB9.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPB9.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PA9
	handlerUserLedPinPA9.pGPIOx = GPIOA;
	handlerUserLedPinPA9.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerUserLedPinPA9.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA9.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA9.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA9.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA9.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PA7
	handlerUserLedPinPA7.pGPIOx = GPIOA;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA7.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA7.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA7.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA7.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PA8
	handlerUserLedPinPA8.pGPIOx = GPIOA;
	handlerUserLedPinPA8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserLedPinPA8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA8.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA8.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC8
	handlerUserLedPinPC8.pGPIOx = GPIOC;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC8.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPC8.GPIO_PinConfig.PinAltFunMode	= AF0; //No function


	//Puerto PA6
	handlerUserLedPinPA6.pGPIOx = GPIOA;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function


	//Puerto PB6
	handlerUserLedPinPB6.pGPIOx = GPIOB;
	handlerUserLedPinPB6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserLedPinPB6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPB6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPB6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPB6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPB6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function


	/*Cargar configuración*/
	GPIO_Config(&handlerUserLedPinPA5);	//Led estado
	GPIO_Config(&handlerUserLedPinPC7); //DispUni
	GPIO_Config(&handlerUserLedPinPA8); //c
	GPIO_Config(&handlerUserLedPinPB6); //d
	GPIO_Config(&handlerUserLedPinPB8); //g
	GPIO_Config(&handlerUserLedPinPB9); //f
	GPIO_Config(&handlerUserLedPinPA9); //e
	GPIO_Config(&handlerUserLedPinPA7); //b
	GPIO_Config(&handlerUserLedPinPC9); //DispDec
	GPIO_Config(&handlerUserLedPinPA6); //a

	//Encender el DispUni
	GPIO_WritePin(&handlerUserLedPinPC7, SET);

	/*Configurar Timers*/
	//Configurar TIM2 Cada 300ms
	handlerBlinkyTimer2.ptrTIMx = TIM2;
	handlerBlinkyTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period = 250; //Interrupción cada 250 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

	//Configurar TIM3 Cada 300ms
	handlerBlinkyTimer3.ptrTIMx = TIM3;
	handlerBlinkyTimer3.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer3.TIMx_Config.TIMx_speed = BTIMER_SPEED_10us;
	handlerBlinkyTimer3.TIMx_Config.TIMx_period = 300; //Interrupción cada 3000 us
	handlerBlinkyTimer3.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer3);

	/*Config EXTI*/
	//Configurar USER bottom
	handlerUserLedPinPC13.pGPIOx = GPIOC;
	handlerUserLedPinPC13.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserLedPinPC13.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserLedPinPC13.GPIO_PinConfig.PinPuPdControl = GPIO_PUPDR_NOTHING;
	//EXTI
	extiConfigPC13.pGPIOHandler = &handlerUserLedPinPC13;
	extiConfigPC13.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigPC13);
}

void callback_extInt13(void){
	if(counter_Exti13 < 99){
		counter_Exti13++;
	}
}

void BasicTimer2_Callback(void){
	//Led de estado Encendido/apagado
	GPIOxTooglePin(&handlerUserLedPinPA5);
}

void BasicTimer3_Callback(void){
	/*Se alterna el encendido y apagado del DispUni y DispDec a una velocidad que
	  aparente ser estatico para el ojo humano*/
	GPIOxTooglePin(&handlerUserLedPinPC7);
	GPIOxTooglePin(&handlerUserLedPinPC9);

	if(GPIO_ReadPin(&handlerUserLedPinPC7)){
		numeroDisplay(~numero[counter_Exti13 / 10]); //Decenas
	}
	else{
		numeroDisplay(~numero[counter_Exti13 % 10]); //Unidades
	}
}

void numeroDisplay(int numero){
	/* Recibe como argunmento numero entero "numero",
	 * muestra en el dispay el numero "numero"
	 */
	GPIO_WritePin(&handlerUserLedPinPB8, ((1 << 0) & numero) >> 0);	//bit-g
	GPIO_WritePin(&handlerUserLedPinPB9, ((1 << 1) & numero) >> 1);	//bit-f
	GPIO_WritePin(&handlerUserLedPinPA9, ((1 << 2) & numero) >> 2);	//bit-e
	GPIO_WritePin(&handlerUserLedPinPB6, ((1 << 3) & numero) >> 3);	//bit-d
	GPIO_WritePin(&handlerUserLedPinPA8, ((1 << 4) & numero) >> 4);	//bit-c
	GPIO_WritePin(&handlerUserLedPinPA7, ((1 << 5) & numero) >> 5);	//bit-b
	GPIO_WritePin(&handlerUserLedPinPA6, ((1 << 6) & numero) >> 6);	//bit-a

}
