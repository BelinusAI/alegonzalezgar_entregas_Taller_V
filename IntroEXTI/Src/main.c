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
GPIO_Handler_t handlerLed2 = {0}; //PA5
GPIO_Handler_t handlerUserB= {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
EXTI_Config_t extiConfigPC13 = {0};

uint32_t counter_EXTI13;

//Prototipos de funciones
void init_Hadware(void);
void callback_exti13(void);

int main(void){
	init_Hadware();

	while(1){

	}

	return 0;

}

void init_Hadware(void){
	//Configurar Led2 -> PA5
	handlerLed2.pGPIOx							= GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerLed2.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargar configuración del pin
	GPIO_Config(&handlerLed2);
	//Encender el Led2
	GPIO_WritePin(&handlerLed2, SET);

	//Configurar TIM2 Cada 300ms
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300; //Interrupción cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/*Config EXTI*/
	//Configurar USER bottom
	handlerUserB.pGPIOx = GPIOC;
	handlerUserB.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserB.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserB.GPIO_PinConfig.PinPuPdControl = GPIO_PUPDR_NOTHING;
	//EXTI
	extiConfigPC13.pGPIOHandler = &handlerUserB;
	extiConfigPC13.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigPC13);

}

void callback_extInt13(void){
	counter_EXTI13++;
	GPIO_WritePin(&handlerLed2, RESET);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLed2);
}
