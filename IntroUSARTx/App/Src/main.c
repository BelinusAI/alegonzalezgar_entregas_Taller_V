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
#include "USARTxDriver.h"
#include <math.h>
#include "PLLDriver.h"


/*Definición de elementos*/
GPIO_Handler_t handlerUserBlinkyPin = {0}; //BlinkyPin

GPIO_Handler_t handlerUserButtom  = {0};
EXTI_Config_t handlerUserButtomExti = {0};

BasicTimer_Handler_t handlerBlinkyTimer2 = {0}; //Led de estado

//Definición USART
GPIO_Handler_t handlerPinTx = {0};
GPIO_Handler_t handlerPinRx = {0};
USART_Handler_t usart1Handler = {0};
uint8_t printMsg = 0;
uint8_t usart2DataReceived = 0;

char mensaje[] = "Mundo \n";
char buffer[64] = {0};

//Prototipos de funciones
void init_Hadware(void);


int main(void){
	init_Hadware();
	configPLL(80);

	while(1){
		if(printMsg > 4){
			/*writeChar(&usart2Handler, 'H');
			writeChar(&usart2Handler, 'O');
			writeChar(&usart2Handler, 'L');
			writeChar(&usart2Handler, 'A');
			writeChar(&usart2Handler, ' ');

			writeMsg(&usart2Handler, mensaje);

			sprintf(buffer, "Valor de printMsg= %d \n", printMsg);
			writeMsg(&usart2Handler, buffer);


			writeMsg(&usart2Handler, buffer);
			*/
			sprintf(buffer,"%d", getConfigPLL());
			writeMsg(&usart1Handler,buffer);
			printMsg = 0;
		}

		if(usart2DataReceived != '\0'){
			//writeChar(&usart2Handler, usart2DataReceived);
			sprintf(buffer, "Recibido = %c \n", usart2DataReceived);
			writeMsg(&usart1Handler, buffer);
			usart2DataReceived = '\0';
		}



	}

	return 0;

}

void init_Hadware(void){
	//Puerto PA5 BlinkyPin
	handlerUserBlinkyPin.pGPIOx 						= GPIOA;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerUserBlinkyPin.GPIO_PinConfig.PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserBlinkyPin.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerUserBlinkyPin.GPIO_PinConfig.PinAltFunMode	= AF0; //No function
	GPIO_Config(&handlerUserBlinkyPin);

	//Configurar TIM2
	handlerBlinkyTimer2.ptrTIMx 							= TIM2;
	handlerBlinkyTimer2.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed 				= BTIMER_80HZ_SPEED_100us;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period 			= 2500; //Interrupción cada 2500 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

	//Puerto PC13 UserButtom
	handlerUserButtom.pGPIOx 						= GPIOC;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinNumber	= PIN_13;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_IN;
	handlerUserButtom.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_PULLUP;
	GPIO_Config(&handlerUserButtom);

	//EXTI
	handlerUserButtomExti.pGPIOHandler 	= &handlerUserButtom;
	handlerUserButtomExti.edgeType 		= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtomExti);

	/*Config USART*/
	// Transmition
	handlerPinTx.pGPIOx 						= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinTx);
	// Reception
	handlerPinRx.pGPIOx 						= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber	= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	usart1Handler.ptrUSARTx 						= USART1;
	usart1Handler.USART_Config.USART_baudrate		= USART_BAUDRATE_80MHZ_19200;
	usart1Handler.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart1Handler.USART_Config.USART_parity			= USART_PARITY_NONE;
	usart1Handler.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	usart1Handler.USART_Config.USART_mode			= USART_MODE_RXTX;
	usart1Handler.USART_Config.USART_EnableIntRX	= USART_RX_INTERRUP_ENABLE;
	usart1Handler.USART_Config.USART_EnableIntTX	= USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart1Handler);


}

void usart2Rx_Callback(void){
	//GPIO_WritePin(&handlerUserBlinkyPin, SET);
	usart2DataReceived = getRxData();
	//GPIO_WritePin(&handlerUserBlinkyPin, RESET);
}

void callback_extInt13(void){
	__NOP();
}

void BasicTimer2_Callback(void){
	//Led de estado Encendido/apagado
	GPIOxTooglePin(&handlerUserBlinkyPin);
	printMsg++;
}





