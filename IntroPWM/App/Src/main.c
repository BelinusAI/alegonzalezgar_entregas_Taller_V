/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : CMSIS-BasicConfig
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"



/*Definición de elementos*/
GPIO_Handler_t handlerUserBlinkyPin = {0}; //BlinkyPin

GPIO_Handler_t handlerUserButtom  = {0};
EXTI_Config_t handlerUserButtomExti = {0};

BasicTimer_Handler_t handlerBlinkyTimer2 = {0}; //Led de estado



//Definición USART
GPIO_Handler_t handlerPinTx = {0};
GPIO_Handler_t handlerPinRx = {0};
USART_Handler_t usart2Handler = {0};
uint8_t printMsg = 0;
uint8_t usart2DataReceived = 0;

// Elementos para PWM
GPIO_Handler_t handlerPinPwmChannel 	= {0};
PWM_Handler_t handlerSignalPWM 			= {0};

uint16_t duttyValue = 5000;

char mensaje[] = "Mundo \n";
char buffer[64] = {0};

float valueA = 13.657f;
float valueB = 345.65f;
float valueC = 0.0f;


//Prototipos de funciones
void init_Hadware(void);


int main(void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	init_Hadware();

	while(1){
		if(usart2DataReceived != '\0'){
			if(usart2DataReceived == 'D'){
				//Down
				duttyValue -= 10;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			if(usart2DataReceived == 'U'){
				duttyValue += 10;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			sprintf(buffer, "dutty = %u \n", (unsigned int)duttyValue);
			writeMsg(&usart2Handler, buffer);

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
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period 			= 250; //Interrupción cada 2500 ms
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
	handlerPinTx.pGPIOx 						= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber	= PIN_3;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinTx);

	handlerPinRx.pGPIOx 						= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber	= PIN_2;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	usart2Handler.ptrUSARTx 						= USART2;
	usart2Handler.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	usart2Handler.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart2Handler.USART_Config.USART_parity			= USART_PARITY_NONE;
	usart2Handler.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	usart2Handler.USART_Config.USART_mode			= USART_MODE_RXTX;
	usart2Handler.USART_Config.USART_EnableIntRX	= USART_RX_INTERRUP_ENABLE;
	usart2Handler.USART_Config.USART_EnableIntTX	= USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart2Handler);

	// Configuración PWM
	handlerPinPwmChannel.pGPIOx							= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber	= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel.GPIO_PinConfig.PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmChannel);

	// Timer señal PWM
	handlerSignalPWM.ptrTIMx 							= 	TIM3;
	handlerSignalPWM.config.channel						=	PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle					= 	duttyValue;
	handlerSignalPWM.config.periodo						= 	20000;
	handlerSignalPWM.config.prescaler					= 	16;

	// Cargamos la configuración
	pwm_Config(&handlerSignalPWM);
	// Activamos la señal
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);


	//Configurar TIM3 Muestreo
	BasicTimer_Handler_t handlerSampleTimer3 = {0};
	handlerSampleTimer3.ptrTIMx 							= TIM3;
	handlerSampleTimer3.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerSampleTimer3.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerSampleTimer3.TIMx_Config.TIMx_period 			= 1; //Interrupción cada 2500 ms
	handlerSampleTimer3.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerSampleTimer3);


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





