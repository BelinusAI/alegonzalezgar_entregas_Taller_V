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

//Definición de elementos
GPIO_Handler_t handlerLed2 = {0}; //PA5
GPIO_Handler_t handlerUserB= {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

uint32_t counter_Exti13 = 0;

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

	//Cargar configuración del Timer
	BasicTimer_Config(&handlerBlinkyTimer);


	/*Config EXTI*/
	//Configurar USER bottom
	handlerUserB.pGPIOx = GPIOC;
	handlerUserB.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserB.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserB.GPIO_PinConfig.PinPuPdControl = GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerUserB);

	/*Activar la señal de reloj SYSCFG*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Configurar Mux 13 */
	SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);
	SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC; //Puerto C MUX 13

	/* Configurando flanco*/
	EXTI->FTSR = 0; //Desactivar flanco de bajada
	EXTI->RTSR = 0;
	EXTI->RTSR |= EXTI_RTSR_TR13; //Activar flanco de subida

	/*Configurar mascara EXTI*/
	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13;


	/*Desactivar Interrupciones*/
	__disable_irq();

	/*Matricular interrupciones en NVIC*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/*Activar Interrupciones*/
		__enable_irq();


}

void callback_exti13(void){
	counter_Exti13++;
}

//Definición ISR para EXTI13
void EXTI15_10_IRQHandler(void){
	if(EXTI->PR & EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;
		//Limpiar bandera
		callback_exti13();
	}
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLed2);
}
