#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define ACCEL_ADDRESS          	 0x53
#define WHO_AM_I                 0
#define BW_RATE					 44
#define POWER_CTL                45
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55



/*Definición de elementos*/
//Led de estado
GPIO_Handler_t handlerUserBlinkyPin = {0};
BasicTimer_Handler_t handlerBlinkyTimer2 = {0};

// USART1
GPIO_Handler_t handlerPinTx = {0};
GPIO_Handler_t handlerPinRx = {0};
USART_Handler_t usart1Handler = {0};
uint8_t rxData = '\0';
char bufferReception[64] = {0};
uint16_t counterReception;
bool stringComplete = false;

char bufferData[64] = "Solución Examen Parcial \n";

// Timer Muestreo
BasicTimer_Handler_t handlerSampleTimer4 = {0}; // Timer Muestreo
uint16_t samples = 0;
uint16_t contador = 0;

// I2C  Acelerometro
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};
uint8_t AccelX_low = 0;
uint8_t AccelX_high = 0;
int16_t AccelX = 0;
uint8_t AccelY_low = 0;
uint8_t AccelY_high = 0;
int16_t AccelY = 0;
uint8_t AccelZ_low = 0;
uint8_t AccelZ_high = 0;
int16_t AccelZ = 0;
uint8_t muestreo = 0;
float datosAcelerometro[6000];

// Cmd config
char cmd[64] = {0};
unsigned int firstParameter;
unsigned int secondParameter;
char userMsg[64] = {0};

//Prototipos de funciones
void init_Hadware(void);
void parseCommands(char *ptrBufferReception);


int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	/* Configuración Clock */
	configPLL(); // Configuración a 100MHz
	config_SysTick_ms(2); // Configuración SysTick 100MHz

	/* Inicio de sistema */
	init_Hadware();

	RCC->CR &= ~(RCC_CR_HSITRIM_Msk);	// Limpiar
	RCC->CR |= (0xD<<RCC_CR_HSITRIM_Pos); // Calibrar reloj -0.9%

	//Imprimir un mensaje de inicio
	writeMsg(&usart1Handler, bufferData);

	while(1){

		if(muestreo){
			AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
			AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);

			AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
			AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);

			AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
			AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);

			AccelX = AccelX_high << 8 | AccelX_low;
			AccelY = AccelY_high << 8 | AccelY_low;
			AccelZ = AccelZ_high << 8 | AccelZ_low;

			muestreo = 0;
		}

		// El caracter '@' nos indica que es el final de la cadena
		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			// If the incoming character is a newline, set a flag
			// so the main loop can do something about it
			if(rxData == '@'){
				stringComplete = true;

				//Agrego esta linea para crear el string con el null al final
				bufferReception[counterReception] = '\0';
				counterReception = 0;
			}
			//Para que no vuelva entrar. Solo cambia debido a la interrupcion
			rxData = '\0';
		}

		//Hacemos un analisis de la cadena de datos obtenida
		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}
	}

	return 0;
}

void init_Hadware(void){
	/* Blinky Pin */
	//Puerto PA5
	handlerUserBlinkyPin.pGPIOx 							= GPIOA;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerUserBlinkyPin.GPIO_PinConfig.PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerUserBlinkyPin.GPIO_PinConfig.PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEEDR_FAST;
	handlerUserBlinkyPin.GPIO_PinConfig.PinAltFunMode		= AF0; //No function
	GPIO_Config(&handlerUserBlinkyPin);

	//Configurar TIM2
	handlerBlinkyTimer2.ptrTIMx 							= TIM2;
	handlerBlinkyTimer2.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed 				= BTIMER_100MHZ_SPEED_10us;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period 			= 25000; //Interrupción cada 250 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

	/*Config USART*/
	// Transmition
	handlerPinTx.pGPIOx 									= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.PinAltFunMode 				= AF7;
	GPIO_Config(&handlerPinTx);
	// Reception
	handlerPinRx.pGPIOx 									= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.PinAltFunMode				= AF7;
	GPIO_Config(&handlerPinRx);
	//USART
	usart1Handler.ptrUSARTx 								= USART1;
	usart1Handler.USART_Config.USART_baudrate				= USART_BAUDRATE_100MHZ_115200;
	usart1Handler.USART_Config.USART_datasize 				= USART_DATASIZE_8BIT;
	usart1Handler.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart1Handler.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	usart1Handler.USART_Config.USART_mode					= USART_MODE_RXTX;
	usart1Handler.USART_Config.USART_EnableIntRX			= USART_RX_INTERRUP_ENABLE;
	usart1Handler.USART_Config.USART_EnableIntTX			= USART_TX_INTERRUP_DISABLE;
	USART_Config(&usart1Handler);

	/* Configuración I2C Acelerometro */
	//SCL
	I2cSCL.pGPIOx                                    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.PinOPType	            	 = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.PinPuPdControl	         = GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEEDR_FAST;
	I2cSCL.GPIO_PinConfig.PinAltFunMode	   		     = AF4;
	GPIO_Config(&I2cSCL);
	//SDA
	I2cSDA.pGPIOx                                    = GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.PinOPType		             = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.PinPuPdControl	         = GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEEDR_FAST;
	I2cSDA.GPIO_PinConfig.PinAltFunMode		         = AF4;
	GPIO_Config(&I2cSDA);
	// I2C1
	Accelerometer.ptrI2Cx                            = I2C1;
	Accelerometer.modeI2C                            = I2C_MODE_FM;
	Accelerometer.slaveAddress                       = ACCEL_ADDRESS;
	Accelerometer.mainClock							 = MAIN_CLOCK_100_MHz_FOR_I2C;
	Accelerometer.maxI2C_FM							 =I2C_MAX_RISE_TIME_SM_100MHZ;
	Accelerometer.modeI2C_FM						 =I2C_MODE_FM_SPEED_400KHz_100MHz;
	i2c_config(&Accelerometer);
	i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D); //Reset
	i2c_writeSingleRegister(&Accelerometer, BW_RATE, 0xE); //Toma de datos 1600Hz

	//Configurar TIM4 Sample
	handlerSampleTimer4.ptrTIMx 							= TIM4;
	handlerSampleTimer4.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerSampleTimer4.TIMx_Config.TIMx_speed 				= BTIMER_100MHZ_SPEED_100us;
	handlerSampleTimer4.TIMx_Config.TIMx_period 			= 10; //Interrupción cada 1 ms
	handlerSampleTimer4.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerSampleTimer4);
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
	muestreo = 1;
}

void parseCommands(char *ptrBufferReception){

	/* Lee la cadena de caracteres a la que apunta el "ptrBufferReception
	 * y almacena en tres elementos diferentes: un string llamado "cmd",
	 * y dos integer llamados "firstParameter" y "secondParameter"
	 * De esta forma podemos introducir informacion al micro desde el puerto serial
	 */
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);
	if (strcmp(cmd, "help") == 0){
		writeMsg(&usart1Handler, "Help Menu CMDs: \n");
		writeMsg(&usart1Handler, "1)  Help -> Print this menu \n");
		writeMsg(&usart1Handler, "2)  MCO signal -> LSE, PLL, HSI \n");
		writeMsg(&usart1Handler, "3)  MCO prescaler -> 0,2,3,4,5 \n");
		writeMsg(&usart1Handler, "4)  RTC \n");
		writeMsg(&usart1Handler, "5)  RTC \n");
		writeMsg(&usart1Handler, "6)  RTC \n");
		writeMsg(&usart1Handler, "7)  RTC \n");
		writeMsg(&usart1Handler, "8)  Datos analogos: Velocidad de muestreo \n");
		writeMsg(&usart1Handler, "9)  Datos analogos: Presentacion de los arreglos \n");
		writeMsg(&usart1Handler, "10) Acelerometro: Captura de datos \n");
		writeMsg(&usart1Handler, "11) Acelerometro: Frecuencias \n");

	}
	// El comando dummy sirve para entender como funciona la recepcion de numeros
	//enviados desde la consola
	else if (strcmp(cmd, "dummy") == 0) {
		writeMsg(&usart1Handler, "CMD: dummy \n");
		// Cambiando el formato para presentar el numero por el puerto serial
		sprintf(bufferData, "number A = %u \n", firstParameter);
		writeMsg(&usart1Handler, bufferData);
		sprintf(bufferData, "number B = %u \n", secondParameter);
		writeMsg(&usart1Handler, bufferData);
	}
	// El comando usermsg sirve para entender como funciona la recepcion de strings
	//enviados desde la consola
	else if (strcmp(cmd, "usermsg") == 0) {
		writeMsg(&usart1Handler, "CMD: usermsg \n");
		writeMsg(&usart1Handler, userMsg);
		writeMsg(&usart1Handler, "\n");
	}
	else if (strcmp(cmd, "setPeriod") == 0) {
		writeMsg(&usart1Handler, "CMD: setPeriod \n");
	}
	else if (strcmp(cmd, "w") == 0) {
		sprintf(bufferData, "WHO_AM_I? (r)\n");
		writeMsg(&usart1Handler, bufferData);

		i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
		sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
		writeMsg(&usart1Handler, bufferData);
		}
	else if (strcmp(cmd, "x") == 0) {
			writeMsg(&usart1Handler, "CMD: x \n");
			//Imprimir valor X
			sprintf(bufferData, "Axis X data (r) \n");
			writeMsg(&usart1Handler, bufferData);
			sprintf(bufferData, "AccelX = %f \n", (float) (AccelX) * 9.8 /16384);
			writeMsg(&usart1Handler, bufferData);
		}
	else if (strcmp(cmd, "y") == 0) {
			writeMsg(&usart1Handler, "CMD: y \n");
			//Imprimir valor Y
			sprintf(bufferData, "Axis Y data (r)\n");
			writeMsg(&usart1Handler, bufferData);
			sprintf(bufferData, "AccelY = %f \n", (float) (AccelY) * 9.8 / 16384);
			writeMsg(&usart1Handler, bufferData);
		}
	else if (strcmp(cmd, "z") == 0) {
			writeMsg(&usart1Handler, "CMD: z \n");
			//Imprimir valor Z
			sprintf(bufferData, "Axis Z data (r)\n");
			writeMsg(&usart1Handler, bufferData);
			sprintf(bufferData, "AccelZ = %f \n", (float) (AccelZ) * 9.8 / 16384);
			writeMsg(&usart1Handler, bufferData);
		}
	else {
		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&usart1Handler, "Wrong CMD \n");
	}
}

