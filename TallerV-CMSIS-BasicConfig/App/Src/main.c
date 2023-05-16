/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : CMSIS-BasicConfig
 ******************************************************************************
 *
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/*Definición de elementos*/
//Puertos GPIOx
GPIO_Handler_t handlerUserBlinkyPin = {0}; //BlinkyPin
GPIO_Handler_t handlerUserButtom  = {0};

//Definición Timers
BasicTimer_Handler_t handlerBlinkyTimer2 = {0}; //Led de estado

//Definición EXTI
EXTI_Config_t handlerUserButtomExti = {0};

//Prototipos de funciones
void init_Hadware(void);


int main(void){
	init_Hadware();

	while(1){

	}

	return 0;

}

void init_Hadware(void){
	//Puerto PA5 BlinkyPin
	handlerUserBlinkyPin.pGPIOx = GPIOA;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserBlinkyPin.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserBlinkyPin.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserBlinkyPin.GPIO_PinConfig.PinAltFunMode	= AF0; //No function
	GPIO_Config(&handlerUserBlinkyPin);

	/*Configurar Timers*/
	//Configurar TIM2
	handlerBlinkyTimer2.ptrTIMx = TIM2;
	handlerBlinkyTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period = 250; //Interrupción cada 250 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

	/*Config EXTI*/
	//Puerto PC5
	handlerUserButtom.pGPIOx = GPIOC;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinNumber	= PIN_13;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserButtom.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLUP;

	//EXTI
	handlerUserButtomExti.pGPIOHandler = &handlerUserButtom;
	handlerUserButtomExti.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtomExti);

}

void callback_extInt13(void){
	__NOP();
}

void BasicTimer2_Callback(void){
	//Led de estado Encendido/apagado
	GPIOxTooglePin(&handlerUserBlinkyPin);
}





