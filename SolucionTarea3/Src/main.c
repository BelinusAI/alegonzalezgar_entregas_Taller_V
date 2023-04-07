/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Tarea3
 ******************************************************************************
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/*Definición de elementos*/
//Puertos GPIOx
GPIO_Handler_t handlerUserBlinkyPin = {0}; //BlinkyPin
GPIO_Handler_t handlerUserLedPinPC7 = {0}; //c
GPIO_Handler_t handlerUserPinDecPC9 = {0}; //DispDec
GPIO_Handler_t handlerUserLedPinPA9 = {0}; //d
GPIO_Handler_t handlerUserLedPinPA8 = {0}; //e
GPIO_Handler_t handlerUserLedPinPB8 = {0}; //b
GPIO_Handler_t handlerUserLedPinPB9 = {0}; //a
GPIO_Handler_t handlerUserLedPinPA7 = {0}; //g
GPIO_Handler_t handlerUserSWPinPC5  = {0}; //SW
GPIO_Handler_t handlerUserUniPinPB6 = {0}; //DispUni
GPIO_Handler_t handlerUserLedPinPA6 = {0}; //f
GPIO_Handler_t handlerUserDTPinPC6  = {0}; //DT
GPIO_Handler_t handlerUserCLKPinPC8 = {0}; //CLK

//Definición Timers
BasicTimer_Handler_t handlerBlinkyTimer2 = {0}; //Led de estado
BasicTimer_Handler_t handlerBlinkyTimer3 = {0}; //Display

//Definición EXTI
EXTI_Config_t extiConfigPC5 = {0}; //Pulsador
EXTI_Config_t extiConfigPC8 = {0}; //Encoder

uint32_t counter_Exti = 0; //Contador
uint32_t culebrita_Exti = 0; //Posición culebrita

char modoContador = 1; //Modo contador/culebrita

//Lista de numeros del uno al nueve para el 7 segmentos
uint16_t num[10] = {0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011,
		0b01011011, 0b01011111, 0b01110000 , 0b01111111, 0b01111011};

//Lista de las posiciones de culebrita
uint16_t culebra[12] = {0b11000000, 0b01000000, 0b00000010, 0b00000100, 0b00001000,
		0b10000100, 0b10000010, 0b00100000 , 0b00010000, 0b10001000, 0b10010000,
		0b10100000};

//Prototipos de funciones
void init_Hadware(void);
void display(uint32_t );


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

	//Puerto PC7
	handlerUserLedPinPC7.pGPIOx = GPIOC;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPC7.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPC7.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPC7.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPC7.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PC6
	handlerUserDTPinPC6.pGPIOx = GPIOC;
	handlerUserDTPinPC6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserDTPinPC6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserDTPinPC6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLUP;

	//Puerto PC9 DispUni
	handlerUserPinDecPC9.pGPIOx = GPIOC;
	handlerUserPinDecPC9.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerUserPinDecPC9.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserPinDecPC9.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserPinDecPC9.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserPinDecPC9.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserPinDecPC9.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

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

	//Puerto PA6
	handlerUserLedPinPA6.pGPIOx = GPIOA;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserLedPinPA6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPinPA6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPinPA6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserLedPinPA6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function

	//Puerto PB6
	handlerUserUniPinPB6.pGPIOx = GPIOB;
	handlerUserUniPinPB6.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerUserUniPinPB6.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserUniPinPB6.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserUniPinPB6.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserUniPinPB6.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserUniPinPB6.GPIO_PinConfig.PinAltFunMode	= AF0; //No function


	/*Cargar configuración*/
	GPIO_Config(&handlerUserBlinkyPin);	//Led estado
	GPIO_Config(&handlerUserLedPinPC7); //c
	GPIO_Config(&handlerUserLedPinPA8); //e
	GPIO_Config(&handlerUserUniPinPB6); // DispUni
	GPIO_Config(&handlerUserLedPinPB8); //b
	GPIO_Config(&handlerUserLedPinPB9); //a
	GPIO_Config(&handlerUserLedPinPA9); //d
	GPIO_Config(&handlerUserLedPinPA7); //g
	GPIO_Config(&handlerUserPinDecPC9); //DispDec
	GPIO_Config(&handlerUserLedPinPA6); //f
	GPIO_Config(&handlerUserDTPinPC6); //PinData


	//Apagar el DispDecpPinHandler
	GPIO_WritePin(&handlerUserPinDecPC9, SET);


	/*Configurar Timers*/
	//Configurar TIM2 CpPinHandlerada 300ms
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
	//Puerto PC5
	handlerUserSWPinPC5.pGPIOx = GPIOC;
	handlerUserSWPinPC5.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserSWPinPC5.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserSWPinPC5.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLUP;

	//EXTI
	extiConfigPC5.pGPIOHandler = &handlerUserSWPinPC5;
	extiConfigPC5.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigPC5);

	//Puerto PC8
	handlerUserCLKPinPC8.pGPIOx = GPIOC;
	handlerUserCLKPinPC8.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerUserCLKPinPC8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserCLKPinPC8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLDOWN;

	//EXTI
	extiConfigPC8.pGPIOHandler = &handlerUserCLKPinPC8;
	extiConfigPC8.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigPC8);
}

void callback_extInt8(void){
	if(modoContador){
		if((GPIO_ReadPin(&handlerUserCLKPinPC8) && GPIO_ReadPin(&handlerUserDTPinPC6))
				&& counter_Exti > 0){
			counter_Exti--; //Disminuir contador
		}else if((GPIO_ReadPin(&handlerUserCLKPinPC8) ^ GPIO_ReadPin(&handlerUserDTPinPC6)) //Desfase
				&& counter_Exti < 99){
			counter_Exti++; //Aumentar contador
		}
	}
	else{
		if((GPIO_ReadPin(&handlerUserCLKPinPC8) && GPIO_ReadPin(&handlerUserDTPinPC6))
				&& culebrita_Exti > 0){
			culebrita_Exti--; //Disminuir posición
		}
		else if((GPIO_ReadPin(&handlerUserCLKPinPC8) ^ GPIO_ReadPin(&handlerUserDTPinPC6)) //Desfase
				&& culebrita_Exti < 12) {
			culebrita_Exti++; //Aumentar posición
		}
	}
}

void callback_extInt5(void){
	//Cambiar el modo
	modoContador = modoContador ^ SET;

}

void BasicTimer2_Callback(void){
	//Led de estado Encendido/apagado
	GPIOxTooglePin(&handlerUserBlinkyPin);
}

void BasicTimer3_Callback(void){
	/*Se alterna el encendido y apagado del DispUni y DispDec a una velocidad que
	  aparente ser estatico para el ojo humano*/

	if(modoContador){
		GPIOxTooglePin(&handlerUserUniPinPB6);
		GPIOxTooglePin(&handlerUserPinDecPC9);

		if(GPIO_ReadPin(&handlerUserUniPinPB6)){
			display(~num[counter_Exti / 10]); //Decenas
		}
		else{
			display(~num[counter_Exti % 10]); //Unidades
		}
	}
	else{
		//Modo culebrita
		GPIO_WritePin(&handlerUserUniPinPB6, ((1 << 7) & ~culebra[culebrita_Exti % 12]) >> 7);
		GPIO_WritePin(&handlerUserPinDecPC9, ((1 << 7) & culebra[culebrita_Exti % 12]) >> 7);

		display(~culebra[culebrita_Exti % 12]);
	}
}

void display(uint32_t  numero){
	/* Recibe como argunmento numero  n,
	 * muestra en el dispay el numero n
	 */
	GPIO_WritePin(&handlerUserLedPinPA7, ((1 << 0) & numero) >> 0);	//bit-g
	GPIO_WritePin(&handlerUserLedPinPA6, ((1 << 1) & numero) >> 1);	//bit-f
	GPIO_WritePin(&handlerUserLedPinPA8, ((1 << 2) & numero) >> 2);	//bit-e
	GPIO_WritePin(&handlerUserLedPinPA9, ((1 << 3) & numero) >> 3);	//bit-d
	GPIO_WritePin(&handlerUserLedPinPC7, ((1 << 4) & numero) >> 4);	//bit-c
	GPIO_WritePin(&handlerUserLedPinPB8, ((1 << 5) & numero) >> 5);	//bit-b
	GPIO_WritePin(&handlerUserLedPinPB9, ((1 << 6) & numero) >> 6);	//bit-a
}


