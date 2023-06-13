#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "I2CDriver.h"
#include "PLLDriver.h"
#include "RtcDriver.h"
#include "arm_math.h"
#include "AdcDriver.h"

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


#define ACCEL_ADDRESS          	 0x53
#define WHO_AM_I                 0
#define BW_RATE					 44
#define POWER_CTL                45
#define ACCEL_XOUT_L             50
#define ACCEL_XOUT_H             51
#define ACCEL_YOUT_L             52
#define ACCEL_YOUT_H             53
#define ACCEL_ZOUT_L             54
#define ACCEL_ZOUT_H             55


/*Definición de elementos*/
//Led de estado
GPIO_Handler_t handlerUserBlinkyPin 		= {0};
BasicTimer_Handler_t handlerBlinkyTimer2	= {0};

// Pin MCO
GPIO_Handler_t handlerPinMCO = {0};

// RTC
RTC_Handler_t handler_RTC = {0};

// USART1
GPIO_Handler_t handlerPinTx 	= {0};
GPIO_Handler_t handlerPinRx 	= {0};
USART_Handler_t usart1Handler 	= {0};
uint8_t rxData 					= '\0';
char bufferReception[64] 		= {0};
uint16_t counterReception;
bool stringComplete 			= false;

char bufferData[64] = {0};

// Timer Muestreo
BasicTimer_Handler_t handlerSampleTimer4 = {0}; // Timer Muestreo
uint16_t samples 	= 0;
uint16_t contador 	= 0;

// I2C  Acelerometro
GPIO_Handler_t I2cSDA 		= {0};
GPIO_Handler_t I2cSCL 		= {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer 			= {0};
uint8_t AccelX_low 			= 0;
uint8_t AccelX_high 		= 0;
int16_t AccelX 				= 0;
uint8_t AccelY_low 			= 0;
uint8_t AccelY_high 		= 0;
int16_t AccelY 				= 0;
uint8_t AccelZ_low 			= 0;
uint8_t AccelZ_high 		= 0;
int16_t AccelZ 				= 0;
uint16_t measure[16] 		= {0};
bool muestreo 				= false;

/* PWM Handler para las señales por PWM */
PWM_Handler_t handlerSignalPWM 			= {0};
uint16_t duttyValue 					= 3;

///* ADC  */
uint32_t datosADC[2][256]			= {0};
uint16_t adcData[2] 				= {0};
ADC_Config_t adcConfig 				= {0};
uint16_t adcDataSingle;
bool adcIsComplete 					= false;
uint8_t counterADC					= 0;
uint16_t numADC 					= 0;


float datosAcelerometroX[128];
float datosAcelerometroY[128];
float datosAcelerometroZ[128];

float transformedSignalY[128];

float32_t stopTime 				= 1.0;
uint32_t ifftFlag 				= 0;
uint32_t doBitReverse 			= 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
//arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT =ARM_MATH_ARGUMENT_ERROR;
uint16_t fttSize 				= 128;


// Cmd config
char cmd[64] = {0};
unsigned int firstParameter;
unsigned int secondParameter;
unsigned int thirdParameter;
char userMsg[64] = {0};

//Prototipos de funciones
void init_Hadware(void);
void parseCommands(char *ptrBufferReception);
void selectMCOSignal(uint16_t param);
void selectMCOpresc(uint16_t prescaler);


int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	/* Configuración Clock */
	configPLL(); 			// Activamos el PLL Configuración a 100MHz
	config_SysTick_ms(2); 	// Configuración SysTick 100MHz

	/* Inicio de sistema */
	init_Hadware();

//	sprintf(bufferData, "Before: 0x%x \n", (unsigned int) ((RCC->CR  & RCC_CR_HSITRIM_Msk) >> RCC_CR_HSITRIM_Pos));
//	writeMsg(&usart1Handler, bufferData);

	/* Sintonizar  la frecuencia del oscilador interno principal (HSI) */
	RCC->CR &= ~(RCC_CR_HSITRIM_Msk);		// Limpiar
	RCC->CR |= (0xD<<RCC_CR_HSITRIM_Pos); 	// Calibrar reloj -0.9%

//	sprintf(bufferData, "After: 0x%x \n", (unsigned int) ((RCC->CR  & RCC_CR_HSITRIM_Msk) >> RCC_CR_HSITRIM_Pos));
//	writeMsg(&usart1Handler, bufferData);

	/* Imprimir un mensaje de inicio */
	writeMsg(&usart1Handler, "Solución Examen Parcial \n");

	while(1){

		// El caracter '/' nos indica que es el final de la cadena
		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			// If the incoming character is a newline, set a flag
			// so the main loop can do something about it
			if(rxData == '/'){
				stringComplete = true;

				//Agrego esta linea para crear el string con el null al final
				bufferReception[counterReception] 	= '\0';
				counterReception 					= 0;
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
	RCC->CR &= ~(RCC_CR_HSEON); // HSE oscillator is OFF

	//Puerto H1
	handlerUserBlinkyPin.pGPIOx 							= GPIOH;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
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
	I2cSCL.pGPIOx                               		    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber            		= PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode             			= GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.PinOPType	            			= GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.PinPuPdControl	         		= GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              		= GPIO_OSPEEDR_FAST;
	I2cSCL.GPIO_PinConfig.PinAltFunMode	   		     		= AF4;
	GPIO_Config(&I2cSCL);
	//SDA
	I2cSDA.pGPIOx                                    		= GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             		= PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               		= GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.PinOPType		             		= GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.PinPuPdControl	         		= GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              		= GPIO_OSPEEDR_FAST;
	I2cSDA.GPIO_PinConfig.PinAltFunMode		         		= AF4;
	GPIO_Config(&I2cSDA);
	// I2C1
	Accelerometer.ptrI2Cx                            		= I2C1;
	Accelerometer.modeI2C                            		= I2C_MODE_FM;
	Accelerometer.slaveAddress                       		= ACCEL_ADDRESS;
	Accelerometer.mainClock							 		= MAIN_CLOCK_100_MHz_FOR_I2C;
	Accelerometer.maxI2C_FM							 		=I2C_MAX_RISE_TIME_SM_100MHZ;
	Accelerometer.modeI2C_FM						 		=I2C_MODE_FM_SPEED_400KHz_100MHz;
	i2c_config(&Accelerometer);
	i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x08); 	//Reset
	i2c_writeSingleRegister(&Accelerometer, BW_RATE, 0xE); 		//Toma de datos 1600Hz

	//Configurar TIM4 Sample
	handlerSampleTimer4.ptrTIMx 							= TIM3;
	handlerSampleTimer4.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerSampleTimer4.TIMx_Config.TIMx_speed 				= BTIMER_100MHZ_SPEED_100us;
	handlerSampleTimer4.TIMx_Config.TIMx_period 			= 50; //Interrupción cada 5 ms 200 Hz
	handlerSampleTimer4.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerSampleTimer4);

	/* Configurar RTC */
	//handler_RTC
	handler_RTC.config.hour 			= 23;
	handler_RTC.config.minutes	 		= 59;
	handler_RTC.config.seconds 			= 50;
	handler_RTC.config.date 			= 01;
	handler_RTC.config.month 			= 01;
	handler_RTC.config.year				= 23;
	config_RTC(&handler_RTC);

	/* Configurar ADC */
	// Timer señal PWM
	handlerSignalPWM.ptrTIMx 									= TIM4;
	handlerSignalPWM.config.channel								= PWM_CHANNEL_4;
	handlerSignalPWM.config.duttyCicle							= 5;
	handlerSignalPWM.config.periodo								= 10;
	handlerSignalPWM.config.prescaler							= 100;

	// Cargamos la configuración
	pwm_Config(&handlerSignalPWM);
	// Activamos la señal
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);

	// Cargando la configuracion para la conversion ADC
	adcConfig.numberOfChannels   								= 2;
	uint8_t channels[2] 		 								= {ADC_CHANNEL_0, ADC_CHANNEL_1};
	adcConfig.channel            								= channels;
	adcConfig.dataAlignment      								= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution         								= ADC_RESOLUTION_12_BIT;
	uint16_t samplingPeriods[2]  								= {0};
	samplingPeriods[0]			 								= ADC_SAMPLING_PERIOD_56_CYCLES;
	samplingPeriods[1]											= ADC_SAMPLING_PERIOD_56_CYCLES;
	adcConfig.samplingPeriod     								= samplingPeriods;
	adcConfig.externalTrigger    								= ADC_EXTERN_TIM_4_CHANNEL_4_RISING;
	adcConfig.mode               								= MULTIPLE;
	adc_Config(&adcConfig);

	/* Configurar MCO Pin para ver la velocidad */
	handlerPinMCO.pGPIOx                             	    	= GPIOA;
	handlerPinMCO.GPIO_PinConfig.GPIO_PinNumber       	    	= PIN_8;
	handlerPinMCO.GPIO_PinConfig.GPIO_PinMode         	    	= GPIO_MODE_ALTFN;
	handlerPinMCO.GPIO_PinConfig.PinOPType	           			= GPIO_OTYPE_PUSHPULL;
	handlerPinMCO.GPIO_PinConfig.GPIO_PinSpeed            		= GPIO_OSPEEDR_FAST;
	handlerPinMCO.GPIO_PinConfig.PinPuPdControl      			= GPIO_PUPDR_NOTHING;
	handlerPinMCO.GPIO_PinConfig.PinAltFunMode       			= AF0;
	GPIO_Config(&handlerPinMCO);
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
	muestreo = true;
	contador++;
}

void adcComplete_Callback(void){
	adcDataSingle = getADC();
	datosADC[counterADC][numADC]= adcDataSingle;

	if(counterADC){
		counterADC = 0;
		numADC++;
	}
	else{
		counterADC = 1;

	}

	if(numADC > 256){
		adcIsComplete = true;
		numADC = 0;
	}

}


void parseCommands(char *ptrBufferReception){

	/* Lee la cadena de caracteres a la que apunta el "ptrBufferReception
	 * y almacena en tres elementos diferentes: un string llamado "cmd",
	 * y dos integer llamados "firstParameter" y "secondParameter"
	 * De esta forma podemos introducir informacion al micro desde el puerto serial
	 */
	sscanf(ptrBufferReception, "%s %u %u %u %s", cmd, &firstParameter, &secondParameter, &thirdParameter, userMsg);
	/* Comando help */
	if (strcmp(cmd, "help") == 0){
		writeMsg(&usart1Handler, "Help Menu CMDs: \n");
		writeMsg(&usart1Handler, "1)  help 									-> Mostrar este menu \n");
		writeMsg(&usart1Handler, "2)  MCOsignal [signal] 				-> Configurar signal 0:PLL 1:HSI 2:LSE\n");
		writeMsg(&usart1Handler, "3)  MCOpres [prescaler] 				-> Configurar prescaler: 0,2,3,4,5 \n");
		writeMsg(&usart1Handler, "4)  getDate 								-> Mostrar la hora y la fecha \n");
		writeMsg(&usart1Handler, "5)  setDate [hora] [minuto] [segundo] 	-> Configurar la fecha \n");
		writeMsg(&usart1Handler, "6)  setDate [año] [mes] [día] 			-> Configurar la fecha \n");
		writeMsg(&usart1Handler, "7)  setSummer 							-> Agregar una hora \n");
		writeMsg(&usart1Handler, "8)  getADC 								-> Presentar los arreglos de datos ADC. \n");
		writeMsg(&usart1Handler, "9)  setPerio [periodo us] 				-> Configurar la velocidad de muestreo de la señal ADC\n");
		writeMsg(&usart1Handler, "10) getData 								-> Adquirir datos acelerometro \n");
		writeMsg(&usart1Handler, "11) getFrequency 							-> Frecuencia de los datos CMSIS-FFT \n");
		writeMsg(&usart1Handler, "12) adjust [scaler] 						-> Sintonizar scaler: 1 - 31 \n");
	}
	/* Cambiar la configuración de la señal  del MCO1 */
	else if (strcmp(cmd, "MCOsignal") == 0) {
		if(firstParameter == 0 || firstParameter == 1 || firstParameter == 2){
			selectMCOSignal(firstParameter);
			writeMsg(&usart1Handler, "Signal configured");
		}
		else{
			writeMsg(&usart1Handler, "Parametro no valido\n");
		}
	}

	else if (strcmp(cmd, "MCOpres") == 0) {
		if(firstParameter == 0 || firstParameter == 2 || firstParameter == 3 || firstParameter == 4 || firstParameter == 5){
			selectMCOpresc(firstParameter);
			stopPwmSignal(&handlerSignalPWM);	sprintf(bufferData, "Prescaler configured: %u", firstParameter);
			writeMsg(&usart1Handler, bufferData);
		}
		else{
			writeMsg(&usart1Handler, "Parametro no valido\n");
		}
	}

	/* Almacenar datos ADC en arreglos de 256 datos */
	else if (strcmp(cmd, "getADC") == 0) {
		startPwmSignal(&handlerSignalPWM);

		if(adcIsComplete){
			stopPwmSignal(&handlerSignalPWM);
			for (int i=0; i<256; i++){
				sprintf(bufferData, "%u ; %u \n", (unsigned int )datosADC[0][i],(unsigned int )datosADC[1][i]);
				writeMsg(&usart1Handler, bufferData);
			}

		}
	}
	/* Cambiar Velocidad de muestreo */
	else if (strcmp(cmd, "setPeriod") == 0) {
		if(firstParameter >0){
			updateFrequency(&handlerSignalPWM, firstParameter);
			updateDuttyCycle(&handlerSignalPWM, firstParameter / 2);
			sprintf(bufferData, "set Periodo %u microsegundos", firstParameter);
			writeMsg(&usart1Handler, bufferData);
		}
		else{
			writeMsg(&usart1Handler, "Periodo no valido\n");
		}
	}
	/* Mostrar la hora y la fecha */
	else if (strcmp(cmd, "getDate") == 0) {
		read_date(bufferData);
		writeMsg(&usart1Handler, bufferData);
	}

	/* Configurar la hora	 */
	else if (strcmp(cmd, "setTime") == 0) {
		if(firstParameter >=0 && firstParameter < 24 && secondParameter >= 0
				&& secondParameter < 60 && thirdParameter >=0 && thirdParameter < 60){
			handler_RTC.config.hour 	= firstParameter;
			handler_RTC.config.minutes = secondParameter;
			handler_RTC.config.seconds = thirdParameter;
			handler_RTC.config.date 	= getDay();
			handler_RTC.config.month 	= getMonth();
			handler_RTC.config.year		= getYear();

			config_RTC(&handler_RTC);

			read_date(bufferData);
			writeMsg(&usart1Handler, bufferData);
		}
		else{
			writeMsg(&usart1Handler, "Time no valido\n");
		}
	}
	/* Configurar la fecha */
	else if (strcmp(cmd, "setDate") == 0) {
		if(firstParameter > 0 && firstParameter < 32 && secondParameter > 0
				&& secondParameter < 13 && thirdParameter >=0 && thirdParameter < 100){
			handler_RTC.config.hour 	= getHour();
			handler_RTC.config.minutes = getMinutes();
			handler_RTC.config.seconds = getSeconds();
			handler_RTC.config.date 	= firstParameter;
			handler_RTC.config.month 	= secondParameter;
			handler_RTC.config.year		= thirdParameter;
			config_RTC(&handler_RTC);
			read_date(bufferData);
			writeMsg(&usart1Handler, bufferData);
		}
		else{
			writeMsg(&usart1Handler, "Date no valido\n");
		}
	}
	/*Add 1 hour (summer time change) */
	else if (strcmp(cmd, "setSummer") == 0) {
		summerTime();
		delay_ms(50);
		read_date(bufferData);
		writeMsg(&usart1Handler, bufferData);

	}

	/* Lanzar la captura de datos del acelerometro */
	else if (strcmp(cmd, "getData") == 0) {
		sprintf(bufferData, "Adquiriendo datos ... \n");
		writeMsg(&usart1Handler, bufferData);
		//i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x08);
		contador = 0;
		muestreo = true;
		while(contador < 128){
			if(muestreo){
				i2c_readMultipleRegister(&Accelerometer, ACCEL_XOUT_L, measure);
				AccelX_low 	= measure[0];
				AccelX_high = measure[1];
				AccelY_low 	= measure[2];
				AccelY_high = measure[3];
				AccelZ_low 	= measure[4];
				AccelZ_high = measure[5];

				AccelX = AccelX_high << 8 | AccelX_low;
				AccelY = AccelY_high << 8 | AccelY_low;
				AccelZ = AccelZ_high << 8 | AccelZ_low;

				datosAcelerometroX[contador] 	= AccelX;
				datosAcelerometroY[contador] 	= AccelY;
				datosAcelerometroZ[contador]	= AccelZ;

				muestreo = false;
			}
		}

		contador = 0;

		//Inicialización FFT
		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fttSize);

		if(statusInitFFT == ARM_MATH_SUCCESS){
			//Imprimir mensaje de exito
			sprintf(bufferData, "Datos adquiridos \n");
			writeMsg(&usart1Handler, bufferData);
		}

		// Imprimir el areglo de 128 datos
		for(int i = 0; i < 128; i++){
			sprintf(bufferData, "n=%d x=%4.2f y=%4.2f z=%4.2f\n", i, datosAcelerometroX[i] * 9.8 / 256, datosAcelerometroY[i]*9.8 / 256, datosAcelerometroZ[i]*9.8 / 256);
			writeMsg(&usart1Handler, bufferData);
		}
	}

	/*  Presentar los datos de la frecuencia leída por el acelerómetro (CMSIS-FFT) */
	else if (strcmp(cmd, "getFrequency") == 0) {
		sprintf(bufferData, "getFrecuency  \n");
		writeMsg(&usart1Handler, bufferData);
		stopTime = 0.0;

		int j = 0;

		sprintf(bufferData, "FFT \n");
		writeMsg(&usart1Handler, bufferData);
		//Calcular la transformada de Fourier
		if(statusInitFFT == ARM_MATH_SUCCESS){
			arm_rfft_fast_f32(&config_Rfft_fast_f32, datosAcelerometroY, transformedSignalY, ifftFlag);
			arm_abs_f32(transformedSignalY, datosAcelerometroY, fttSize);

			// Hallar el máximo e imprimir datos en el dominio de las frecuencias
			uint32_t indexMax = 0;
			float FTT_Max = datosAcelerometroY[1];
			for(int i = 1; i < fttSize; i++){
				if(i%2){
					if(datosAcelerometroY[i] > FTT_Max){
						FTT_Max = datosAcelerometroY[i];
						indexMax = j;
					}
					sprintf(bufferData, "%u ; %#.6f\n", j, 2*datosAcelerometroY[i]);
					writeMsg(&usart1Handler, bufferData);
					j++;
				}
			}

//			sprintf(bufferData, "index %d ; result %f\n,", (int)indexMax, FTT_Max);
//			writeMsg(&usart1Handler, bufferData);

			// Calcular la frecuencia
			float w = (indexMax) / ((fttSize) * 0.005);
			sprintf(bufferData, "frecuency w %f Hz \n,", w);
			writeMsg(&usart1Handler, bufferData);

		}
		else{
			writeMsg(&usart1Handler, "FFT not initialized... \n");
		}
	}


	else if (strcmp(cmd, "adjust") == 0) {
			if(firstParameter > 0 && firstParameter < 32){
				RCC->CR &= ~(RCC_CR_HSITRIM_Msk);					// Limpiar
				RCC->CR |= (firstParameter<<RCC_CR_HSITRIM_Pos); 	// Calibrar reloj
				writeMsg(&usart1Handler, "Adjust\n");
			}
			else{
				writeMsg(&usart1Handler, "Parametro invalido\n");
			}
		}

	else {
		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
		sprintf(bufferData, "Wrong CMD \n");
		writeMsg(&usart1Handler, bufferData);
		writeMsg(&usart1Handler, cmd);
		writeMsg(&usart1Handler, "\n");

	}

	firstParameter  = 0;
	secondParameter = 1000;
	secondParameter = 1000;

}

void selectMCOSignal(uint16_t param){
	if(param==0){
		// Seleccionamos la senal PLL
		RCC -> CFGR &= ~RCC_CFGR_MCO1;
		RCC -> CFGR |= RCC_CFGR_MCO1_0;
		RCC -> CFGR |= RCC_CFGR_MCO1_1;
	} else if (param==1){
		// Seleccionamos la senal HSI
		RCC -> CFGR &= ~RCC_CFGR_MCO1;
	} else if(param==2){
		// Seleccionamos la senal LSE
		RCC -> CFGR &= ~RCC_CFGR_MCO1;
		RCC -> CFGR |= RCC_CFGR_MCO1_0;
		RCC -> CFGR &= ~RCC_CFGR_MCO1_1;

	}
}

void selectMCOpresc(uint16_t prescaler){
	if (prescaler ==0){
		RCC -> CFGR &= ~RCC_CFGR_MCO1PRE;
	}
	if(prescaler ==2){
		// Utilizamos un prescaler para poder ver la senal en el osciloscopio
		RCC -> CFGR &= ~RCC_CFGR_MCO1PRE;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
	}
	else if(prescaler ==3){
		// Utilizamos un prescaler para poder ver la senal en el osciloscopio
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_0;
		RCC -> CFGR &= ~RCC_CFGR_MCO1PRE_1;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
	}
	else if(prescaler ==4){
		// Utilizamos un prescaler para poder ver la senal en el osciloscopio
		RCC -> CFGR &= ~RCC_CFGR_MCO1PRE_0;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_1;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
	}
	else if(prescaler ==5){
		// Utilizamos un prescaler para poder ver la senal en el osciloscopio
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_0;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_1;
		RCC -> CFGR |= RCC_CFGR_MCO1PRE_2;
	}
}

