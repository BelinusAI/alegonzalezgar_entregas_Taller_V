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
BasicTimer_Handler_t handlerSampleTimer4 = {0}; // Timer Muestreo
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
float datosAcelerometro[6000];
uint8_t AccelX_low = 0;
uint8_t AccelX_high = 0;
int16_t AccelX = 0;
uint8_t AccelY_low = 0;
uint8_t AccelY_high = 0;
int16_t AccelY = 0;
uint8_t AccelZ_low = 0;
uint8_t AccelZ_high = 0;
int16_t AccelZ = 0;
uint8_t muestra = 0;

//LCD
GPIO_Handler_t		 handlerLCDSCL			= {0};
GPIO_Handler_t		 handlerLCDSDA			= {0};
I2C_Handler_t		 handlerLCD				= {0};
char LCDdata[80] = {0};

// Elementos para PWM
GPIO_Handler_t handlerPinPwmChannel4PC9X	= {0};
PWM_Handler_t handlerSignalPWMPC9X			= {0};

GPIO_Handler_t handlerPinPwmChannel3PC8Y	= {0};
PWM_Handler_t handlerSignalPWMPC8Y			= {0};

GPIO_Handler_t handlerPinPwmChannel1PC6Z	= {0};
PWM_Handler_t handlerSignalPWMPC6Z			= {0};

uint16_t duttyValueX = 10000;
uint16_t duttyValueY = 10000;
uint16_t duttyValueZ = 10000;


//Prototipos de funciones
void init_Hadware(void);
void PwmSignals(int16_t AccelX, int16_t AccelY, int16_t AccelZ);

int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	config_SysTick_ms(0);
	init_Hadware();

	//Imprimir un mensaje de inicio
	writeMsg(&usart1Handler, bufferData);

	//Inicializar LCD
	LCD_Clear_Screen(&handlerLCD);
	delay_ms(500);
	LCD_Init(&handlerLCD);
	delay_ms(50);
	LCD_Clear(&handlerLCD);
	delay_ms(500);;
;
	//Imprimir Mensaje LCD
	LCD_setCursor(&handlerLCD, 0, 0);
	LCD_sendSTR(&handlerLCD, "Ax =");
	LCD_setCursor(&handlerLCD, 1, 0);
	LCD_sendSTR(&handlerLCD, "Ay = ");
	LCD_setCursor(&handlerLCD, 2, 0);
	LCD_sendSTR(&handlerLCD, "Az = ");
	LCD_setCursor(&handlerLCD, 3, 0);
	LCD_sendSTR(&handlerLCD, "BW_RATE=    1600Hz");


	while(1){

		//Actualizar DuttyValue del PWM
		PwmSignals(AccelX, AccelY, AccelZ);
		//Mmuestreo constante a 1KHz de los tres ejes del accelerometro
		if(muestra){
			AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
			AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);

			AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
			AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);

			AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
			AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);

			AccelX = AccelX_high << 8 | AccelX_low;
			AccelY = AccelY_high << 8 | AccelY_low;
			AccelZ = AccelZ_high << 8 | AccelZ_low;


			muestra = 0;
		}

		//Actualizar pantalla LED cada 1 segundo
		if(contador > 1000){
			AccelX = AccelX_high << 8 | AccelX_low;
			AccelY = AccelY_high << 8 | AccelY_low;
			AccelZ = AccelZ_high << 8 | AccelZ_low;

			sprintf(LCDdata,"%9.2f m/s2",(AccelX*9.78)/256);
			LCD_setCursor(&handlerLCD, 0, 6);
			LCD_sendSTR(&handlerLCD, LCDdata);
			sprintf(LCDdata,"%9.2f m/s2",(AccelY*9.78)/256);
			LCD_setCursor(&handlerLCD, 1, 6);
			LCD_sendSTR(&handlerLCD, LCDdata);
			sprintf(LCDdata,"%9.2f m/s2",(AccelZ*9.78)/256);
			LCD_setCursor(&handlerLCD, 2, 6);
			LCD_sendSTR(&handlerLCD, LCDdata);

			contador = 0;
		}

		// USART reception
		if(rxData != '\0'){
			writeChar(&usart1Handler, rxData);
			// Imprime direccion del acelerometro
			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usart1Handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart1Handler, bufferData);
			}
			//Imprime el estado PWR
			else if (rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usart1Handler, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, POWER_CTL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart1Handler, bufferData);
			}
			// Reset
			else if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&usart1Handler, bufferData);

				i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);
			}
			else if (rxData == 'd'){
				contador = 0;

				while(contador < 2000){
					/*muestreo que captura durante 2s los datos de los 3 ejes del acelerómetro
					 * (6 mil datos en total, 2000 datos por eje, cada eje muestreado a 1KHz) */
					if(muestra){
						AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
						AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);
						AccelX = AccelX_high << 8 | AccelX_low;
						AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
						AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);
						AccelY = AccelY_high << 8 | AccelY_low;
						AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
						AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);
						AccelZ = AccelZ_high << 8 | AccelZ_low;

						datosAcelerometro[contador*3] = (AccelX*9.8) / 256;
						datosAcelerometro[contador*3 + 1] = (AccelY*9.8) / 256;
						datosAcelerometro[contador*3 + 2] = (AccelZ*9.8) / 256;
						muestra = 0;

					}


				}
				// Imprimir el areglo de 6000 datos
				for(int i = 0; i < 2000; i++){
					sprintf(bufferData, "n=%d x=%4.2f y=%4.2f z=%4.2f\n", i, datosAcelerometro[i*3], datosAcelerometro[i*3 +1], datosAcelerometro[i*3 +2]);
					writeMsg(&usart1Handler, bufferData);
				}
				contador = 0;
			}
			else if (rxData == 'x'){
				//Imprimir valor X
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&usart1Handler, bufferData);
				AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %f \n", (float) (AccelX) /256);
				writeMsg(&usart1Handler, bufferData);
			}
			else if(rxData == 'y'){
				//Imprimir valor Y
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usart1Handler, bufferData);
				AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %f \n", (float) (AccelY) / 256);
				writeMsg(&usart1Handler, bufferData);
			}
			else if(rxData == 'z'){
				//Imprimir valor Z
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
	usart1Handler.USART_Config.USART_baudrate		= USART_BAUDRATE_115200;
	usart1Handler.USART_Config.USART_datasize 		= USART_DATASIZE_8BIT;
	usart1Handler.USART_Config.USART_parity			= USART_PARITY_NONE;
	usart1Handler.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	usart1Handler.USART_Config.USART_mode			= USART_MODE_RXTX;
	usart1Handler.USART_Config.USART_EnableIntRX	= USART_RX_INTERRUP_ENABLE;
	usart1Handler.USART_Config.USART_EnableIntTX	= USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart1Handler);

	//Configurar TIM4 Sample
	handlerSampleTimer4.ptrTIMx 							= TIM4;
	handlerSampleTimer4.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerSampleTimer4.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_10us;
	handlerSampleTimer4.TIMx_Config.TIMx_period 			= 100; //Interrupción cada 1 ms
	handlerSampleTimer4.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerSampleTimer4);


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
	Accelerometer.mainClock							 = MAIN_CLOCK_16_MHz_FOR_I2C;
	Accelerometer.maxI2C_FM							 =I2C_MAX_RISE_TIME_FM_16MHz;
	Accelerometer.modeI2C_FM						 =I2C_MODE_FM_SPEED_400KHz_16MHz;
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
	handlerLCD.modeI2C					= I2C_MODE_FM;
	handlerLCD.slaveAddress				= LCD_ADDRESS;
	handlerLCD.mainClock			    = MAIN_CLOCK_16_MHz_FOR_I2C;
	handlerLCD.maxI2C_FM			    =I2C_MAX_RISE_TIME_FM_16MHz;
	handlerLCD.modeI2C_FM				=I2C_MODE_FM_SPEED_400KHz_16MHz;
	i2c_config(&handlerLCD);
	i2c_writeSingleRegister(&Accelerometer, BW_RATE, 0xE); //Toma de datos 1600Hz


	// Configuración PWM
	handlerPinPwmChannel4PC9X.pGPIOx							= GPIOC;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.GPIO_PinNumber	= PIN_9;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel4PC9X.GPIO_PinConfig.PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmChannel4PC9X);


	handlerPinPwmChannel3PC8Y.pGPIOx							= GPIOC;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.GPIO_PinNumber	= PIN_8;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel3PC8Y.GPIO_PinConfig.PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmChannel3PC8Y);


	handlerPinPwmChannel1PC6Z.pGPIOx							= GPIOC;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel1PC6Z.GPIO_PinConfig.PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmChannel1PC6Z);

	// Timer señal PWM Timer3 Channel 4
	handlerSignalPWMPC9X.ptrTIMx 							= 	TIM3;
	handlerSignalPWMPC9X.config.channel						=	PWM_CHANNEL_4;
	handlerSignalPWMPC9X.config.duttyCicle					= 	duttyValueX;
	handlerSignalPWMPC9X.config.periodo						= 	20000;
	handlerSignalPWMPC9X.config.prescaler					= 	16;
	// Cargamos la configuración
	pwm_Config(&handlerSignalPWMPC9X);
	// Activamos la señal
	enableOutput(&handlerSignalPWMPC9X);
	startPwmSignal(&handlerSignalPWMPC9X);

	// Timer señal PWM Timer3 Channel 3
	handlerSignalPWMPC8Y.ptrTIMx 							= 	TIM3;
	handlerSignalPWMPC8Y.config.channel						=	PWM_CHANNEL_3;
	handlerSignalPWMPC8Y.config.duttyCicle					= 	duttyValueY;
	handlerSignalPWMPC8Y.config.periodo						= 	20000;
	handlerSignalPWMPC8Y.config.prescaler					= 	16;
	// Cargamos la configuración
	pwm_Config(&handlerSignalPWMPC8Y);
	// Activamos la señal
	enableOutput(&handlerSignalPWMPC8Y);
	startPwmSignal(&handlerSignalPWMPC8Y);

	// Timer señal PWM Timer3 Channel 1
	handlerSignalPWMPC6Z.ptrTIMx 							= 	TIM3;
	handlerSignalPWMPC6Z.config.channel						=	PWM_CHANNEL_1;
	handlerSignalPWMPC6Z.config.duttyCicle					= 	duttyValueZ;
	handlerSignalPWMPC6Z.config.periodo						= 	20000;
	handlerSignalPWMPC6Z.config.prescaler					= 	16;
	// Cargamos la configuración
	pwm_Config(&handlerSignalPWMPC6Z);
	// Activamos la señal
	enableOutput(&handlerSignalPWMPC6Z);
	startPwmSignal(&handlerSignalPWMPC6Z);

}

void BasicTimer2_Callback(void){
	//Pin de estado
	GPIOxTooglePin(&handlerUserBlinkyPin);
}

void usart1Rx_Callback(void){
	//Interrupción por transmisión
	rxData = getRxData();
}

void BasicTimer4_Callback(void){
	contador++;
	muestra = 1;

}

/*Cada una de las señales PWM depende del estado de cada uno de los valores de los
 * ejes del sensor acelerómetro . Para valores cercanos a cero, el PWM debe estar
 * en el 50% del duty, mientras que para valores negativos de aceleración deberá
 *  estar por debajo de 50% y para valores positivos por encima de 50%.*/
void PwmSignals(int16_t AccelX, int16_t AccelY, int16_t AccelZ){
	float x = (AccelX*9.78)/256;
	float y = (AccelX*9.78)/256;
	float z = (AccelX*9.78)/256;

	if(x > -20 && x < 20){
		duttyValueX = 10000;
	}else if (x<=-20){
		duttyValueX = 5000;
	}else if (x>=20){
		duttyValueX = 15000;
	}

	if(y > -20 && y < 20){
		duttyValueY = 10000;
	}else if (y<=-20){
		duttyValueY = 5000;
	}else if (y>=20){
		duttyValueY = 15000;
	}
	if(z > -20 && z < 20){
		duttyValueZ = 10000;
	}else if (z<=-20){
		duttyValueZ = 5000;
	}else if (z>=20){
		duttyValueZ = 15000;
	}
	updateDuttyCycle(&handlerSignalPWMPC9X, duttyValueX);
	updateDuttyCycle(&handlerSignalPWMPC8Y, duttyValueY);
	updateDuttyCycle(&handlerSignalPWMPC6Z, duttyValueZ);
}
