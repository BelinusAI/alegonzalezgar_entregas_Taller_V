#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "I2CDriver.h"
#include "LCDDriver.h"

#include "stm32f4xx.h"

#include <stdint.h>
#include <stdio.h>

#define LCD_ADDRESS		0x23 //0100011

#define ACCEL_ADDRESS          	 0x53
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55

#define BW_RATE					 44

#define POWER_CTL                45
#define WHO_AM_I                 0


/*Definición de elementos*/
GPIO_Handler_t handlerUserBlinkyPin = {0}; //BlinkyPin
BasicTimer_Handler_t handlerBlinkyTimer2 = {0}; //Led de estado

// Timer Muestreo
BasicTimer_Handler_t handlerSampleTimer3 = {0}; // Timer Muestreo
uint16_t samples = 0;
uint16_t contador = 0;

// USART1
GPIO_Handler_t handlerPinTx = {0};
GPIO_Handler_t handlerPinRx = {0};
USART_Handler_t usart1Handler = {0};
uint8_t rxData = '\0';
char bufferData[64] = "Solucion Tarea Especial Taller5";

/* Configuración para el I2C */
// Acelerometro
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};
int16_t datosAcelerometro[6000];
uint8_t AccelX_low = 0;
uint8_t AccelX_high = 0;
int16_t AccelX = 0;
uint8_t AccelY_low = 0;
uint8_t AccelY_high = 0;
int16_t AccelY = 0;
uint8_t AccelZ_low = 0;
uint8_t AccelZ_high = 0;
int16_t AccelZ = 0;

//LCD
GPIO_Handler_t		 handlerLCDSCL			= {0};
GPIO_Handler_t		 handlerLCDSDA			= {0};
I2C_Handler_t		 handlerLCD				= {0};
char LCDdata[81] = {0};


//Prototipos de funciones
void init_Hadware(void);

int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	config_SysTick_ms(0);
	init_Hadware();

	//Imprimir un mensaje de inicio
	writeMsg(&usart1Handler, bufferData);

	LCD_Init(&handlerLCD);
	LCD_Clear(&handlerLCD);
	delay_ms(1500);



	sprintf(LCDdata, "x =                 z =                 y =                 parametros:         ");
	LCD_sendSTR(&handlerLCD, LCDdata);

	while(1){

		AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
		AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);

		AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
		AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);

		AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
		AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);

		if(contador > 100){
			AccelX = AccelX_high << 8 | AccelX_low;
			AccelY = AccelY_high << 8 | AccelY_low;
			AccelZ = AccelZ_high << 8 | AccelZ_low;


			sprintf(LCDdata, "x = %11.4f m/s2y = %11.4f m/s2z = %11.4f m/s2BW:1600Hz Sensity:2g", (float)(9.8 * AccelX / 1024.), (float)(9.8 * AccelY / 1024.), (float)(9.8 * AccelZ / 1024.));
			LCD_sendSTR(&handlerLCD, LCDdata);

			contador = 0;
		}

		if(rxData != '\0'){
			writeChar(&usart1Handler, rxData);
			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usart1Handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart1Handler, bufferData);
			}
			else if (rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usart1Handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, POWER_CTL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart1Handler, bufferData);
			}
			else if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&usart1Handler, bufferData);

				i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);
			}
			else if (rxData == 'x'){
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&usart1Handler, bufferData);
				AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %f \n", (float) (AccelX) /256);
				writeMsg(&usart1Handler, bufferData);
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usart1Handler, bufferData);
				AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %f \n", (float) (AccelY) / 256);
				writeMsg(&usart1Handler, bufferData);
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&usart1Handler, bufferData);
				AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %f \n", (float)(AccelZ) / 256);
				writeMsg(&usart1Handler, bufferData);
			}
			rxData = '\0';
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
	handlerBlinkyTimer2.TIMx_Config.TIMx_period 			= 250; //Interrupción cada 250 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

	/*Config USART*/
	handlerPinTx.pGPIOx 						= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.PinAltFunMode 	= AF7;
	GPIO_Config(&handlerPinTx);

	handlerPinRx.pGPIOx 						= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber	= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	usart1Handler.ptrUSARTx 						= USART1;
	usart1Handler.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	usart1Handler.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart1Handler.USART_Config.USART_parity			= USART_PARITY_NONE;
	usart1Handler.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	usart1Handler.USART_Config.USART_mode			= USART_MODE_RXTX;
	usart1Handler.USART_Config.USART_EnableIntRX	= USART_RX_INTERRUP_ENABLE;
	usart1Handler.USART_Config.USART_EnableIntTX	= USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart1Handler);

	//Configurar TIM3 Sample
	handlerSampleTimer3.ptrTIMx 							= TIM3;
	handlerSampleTimer3.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerSampleTimer3.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_10us;
	handlerSampleTimer3.TIMx_Config.TIMx_period 			= 1000; //Interrupción cada 1 ms
	handlerSampleTimer3.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerSampleTimer3);


	//Configuración I2C
	//Acelerometro
	I2cSCL.pGPIOx                                    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.PinOPType	            	 = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.PinPuPdControl	         = GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEEDR_FAST;
	I2cSCL.GPIO_PinConfig.PinAltFunMode	   		     = AF4;
	GPIO_Config(&I2cSCL);

	I2cSDA.pGPIOx                                    = GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.PinOPType		             = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.PinPuPdControl	         = GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEEDR_FAST;
	I2cSDA.GPIO_PinConfig.PinAltFunMode		         = AF4;
	GPIO_Config(&I2cSDA);

	Accelerometer.ptrI2Cx                            = I2C1;
	Accelerometer.modeI2C                            = I2C_MODE_FM;
	Accelerometer.slaveAddress                       = ACCEL_ADDRESS;
	i2c_config(&Accelerometer);

	// LCD
	/* Configurando los pines sobre los que funciona el I2C3 */
	handlerLCDSCL.pGPIOx								= GPIOA;
	handlerLCDSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerLCDSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerLCDSCL.GPIO_PinConfig.PinOPType		        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDSCL.GPIO_PinConfig.PinPuPdControl	        = GPIO_PUPDR_NOTHING;
	handlerLCDSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
	handlerLCDSCL.GPIO_PinConfig.PinAltFunMode			= AF4;
	GPIO_Config(&handlerLCDSCL);

	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerLCDSDA.pGPIOx								= GPIOB;
	handlerLCDSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerLCDSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerLCDSDA.GPIO_PinConfig.PinOPType		        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDSDA.GPIO_PinConfig.PinPuPdControl	        = GPIO_PUPDR_NOTHING;
	handlerLCDSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
	handlerLCDSDA.GPIO_PinConfig.PinAltFunMode			= AF9;
	GPIO_Config(&handlerLCDSDA);

	// LCD
	handlerLCD.ptrI2Cx					= I2C3;
	handlerLCD.modeI2C					= I2C_MODE_SM;
	handlerLCD.slaveAddress				= LCD_ADDRESS;
	i2c_config(&handlerLCD);
	i2c_writeSingleRegister(&Accelerometer, BW_RATE, 0xE); //Toma de datos 1600Hz

}

void BasicTimer2_Callback(void){
	//Pin de estado
	GPIOxTooglePin(&handlerUserBlinkyPin);
}

void usart1Rx_Callback(void){
	//Interrupción por transmisión
	rxData = getRxData();
}

void BasicTimer3_Callback(void){
	contador++;

}
