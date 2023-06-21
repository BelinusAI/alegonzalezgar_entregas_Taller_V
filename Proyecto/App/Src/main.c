#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"
#include "AdcDriver.h"
#include "PwmDriver.h"
#include "stdbool.h"
#include "stdlib.h"

#include "stm32f4xx.h"
#include "SH1106.h"

#define SCREEN_WIDTH 		127
#define SCREEN_HEIGHT 		63
#define PADDLE_WIDTH 		4
#define PADDLE_HEIGHT 		10
#define PADDLE_PADDING 		10
#define BALL_SIZE 			3
#define SCORE_PADDING 		10

#define EFFECT_SPEED 		0.2
#define MIN_Y_SPEED 		0.6
#define MAX_Y_SPEED 		2

/*Definición de elementos*/
//Led de estado
GPIO_Handler_t handlerUserBlinkyPin 		= {0};
BasicTimer_Handler_t handlerBlinkyTimer2	= {0};

// RTC
//RTC_Handler_t handler_RTC = {0};

//// USART1
//GPIO_Handler_t handlerPinTx 	= {0};
//GPIO_Handler_t handlerPinRx 	= {0};
//USART_Handler_t usart1Handler 	= {0};
//uint8_t rxData 					= '\0';
//char bufferReception[64] 		= {0};
//uint16_t counterReception;
//bool stringComplete 			= false;
//
//char bufferData[64] = {0};

//// Timer Muestreo
//BasicTimer_Handler_t handlerSampleTimer4 = {0}; // Timer Muestreo
//uint16_t samples 	= 0;
//uint16_t contador 	= 0;

// I2C  OLED
GPIO_Handler_t I2cSDA 						= {0};
GPIO_Handler_t I2cSCL 						= {0};
I2C_Handler_t handler_OLED					= {0};
uint8_t i2cBuffer 							= {0};

// Elementos para PWM Zumbador
GPIO_Handler_t handlerPinPwmChannel3PC8		= {0};
PWM_Handler_t handlerSignalPWMPC8			= {0};


/* PWM Handler para las señales por PWM */
PWM_Handler_t handlerSignalPWM 				= {0};


///* ADC  */
ADC_Config_t adcConfig 						= {0};
uint16_t adcData[2] 						= {0};
uint8_t counterADC							= 0;
uint16_t adcDataSingle;


//// Cmd config
//char cmd[64] = {0};
//unsigned int firstParameter;
//unsigned int secondParameter;
//unsigned int thirdParameter;
//char userMsg[64] = {0};

/* PONG */
int paddleLocationA			= 30;
int paddleLocationB 		= 30;
float ballX 				= SCREEN_WIDTH/2;
float ballY 				= SCREEN_HEIGHT/2;
int scoreA 					= 0;
int scoreB 					= 0;
int lastPaddleLocationA 	= 0;
int lastPaddleLocationB 	= 0;
float ballSpeedX 			= 2;
float ballSpeedY 			= 1;
int paddleSpeedA 			= 0;
int paddleSpeedB 			= 0;

//Prototipos de funciones
void init_Hadware(void);
void draw(void);
void ball(void);
void soundBounce(void);
void soundPoint(void);
void soundStart(void);
void addEffect(int paddleSpeed);
//void parseCommands(char *ptrBufferReception);


int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

//	/* Configuración Clock */
//	configPLL(); 			// Activamos el PLL Configuración a 100MHz
//	config_SysTick_ms(2); 	// Configuración SysTick 100MHz

	/* Inicio de sistema */
	init_Hadware();
	OLED_Init(&handler_OLED);
	startMenu(&handler_OLED);
	delay_ms(1000);
	soundStart();
	draw();
	UpdateDisplay(&handler_OLED);


//	sprintf(bufferData, "Before: 0x%x \n", (unsigned int) ((RCC->CR  & RCC_CR_HSITRIM_Msk) >> RCC_CR_HSITRIM_Pos));
//	writeMsg(&usart1Handler, bufferData);

//	/* Sintonizar  la frecuencia del oscilador interno principal (HSI) */
//	RCC->CR &= ~(RCC_CR_HSITRIM_Msk);		// Limpiar
//	RCC->CR |= (0xD<<RCC_CR_HSITRIM_Pos); 	// Calibrar reloj -0.9%

//	sprintf(bufferData, "After: 0x%x \n", (unsigned int) ((RCC->CR  & RCC_CR_HSITRIM_Msk) >> RCC_CR_HSITRIM_Pos));
//	writeMsg(&usart1Handler, bufferData);

//	/* Imprimir un mensaje de inicio */
//	writeMsg(&usart1Handler, "Solución Examen Parcial \n");

	while(1){

		ball();
		draw();
		UpdateDisplay(&handler_OLED);

		 if (scoreA >= 6){ //gana jugador A al anotar 6 puntos
			 delay_ms(3000);
			 startMenu(&handler_OLED);
			 return 0; //TODO
		 }

		 if (scoreB >= 6){ //gana jugador B al anotar 6 puntos
			 delay_ms(3000);
			 startMenu(&handler_OLED);
			 return 0; //TODO
		 }
//		// El caracter '/' nos indica que es el final de la cadena
//		if(rxData != '\0'){
//			bufferReception[counterReception] = rxData;
//			counterReception++;
//
//			// If the incoming character is a newline, set a flag
//			// so the main loop can do something about it
//			if(rxData == '/'){
//				stringComplete = true;
//
//				//Agrego esta linea para crear el string con el null al final
//				bufferReception[counterReception] 	= '\0';
//				counterReception 					= 0;
//			}
//			//Para que no vuelva entrar. Solo cambia debido a la interrupcion
//			rxData = '\0';
//		}
//
//		//Hacemos un analisis de la cadena de datos obtenida
//		if (stringComplete){
//			parseCommands(bufferReception);
//			stringComplete = false;
//		}
	}
	return 0;
}


void init_Hadware(void){

	/* Blinky Pin */
	//Puerto A5
	handlerUserBlinkyPin.pGPIOx 							= GPIOA;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerUserBlinkyPin.GPIO_PinConfig.PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerUserBlinkyPin.GPIO_PinConfig.PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEEDR_FAST;
	handlerUserBlinkyPin.GPIO_PinConfig.PinAltFunMode		= AF0; //No function
	GPIO_Config(&handlerUserBlinkyPin);

	//Configurar BlinkyTimer TIM2
	handlerBlinkyTimer2.ptrTIMx 							= TIM2;
	handlerBlinkyTimer2.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer2.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerBlinkyTimer2.TIMx_Config.TIMx_period 			= 250; //Interrupción cada 250 ms
	handlerBlinkyTimer2.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer2);

//	/*Config USART*/
//	// Transmition
//	handlerPinTx.pGPIOx 									= GPIOA;
//	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
//	handlerPinTx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinTx.GPIO_PinConfig.PinAltFunMode 				= AF7;
//	GPIO_Config(&handlerPinTx);
//	// Reception
//	handlerPinRx.pGPIOx 									= GPIOA;
//	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
//	handlerPinRx.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerPinRx.GPIO_PinConfig.PinAltFunMode				= AF7;
//	GPIO_Config(&handlerPinRx);
//	//USART
//	usart1Handler.ptrUSARTx 								= USART1;
//	usart1Handler.USART_Config.USART_baudrate				= USART_BAUDRATE_100MHZ_115200;
//	usart1Handler.USART_Config.USART_datasize 				= USART_DATASIZE_8BIT;
//	usart1Handler.USART_Config.USART_parity					= USART_PARITY_NONE;
//	usart1Handler.USART_Config.USART_stopbits				= USART_STOPBIT_1;
//	usart1Handler.USART_Config.USART_mode					= USART_MODE_RXTX;
//	usart1Handler.USART_Config.USART_EnableIntRX			= USART_RX_INTERRUP_ENABLE;
//	usart1Handler.USART_Config.USART_EnableIntTX			= USART_TX_INTERRUP_DISABLE;
//	USART_Config(&usart1Handler);

	/* Configuración I2C OLED */
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
	handler_OLED.ptrI2Cx                            		= I2C1;
	handler_OLED.modeI2C                            		= I2C_MODE_FM;
//	handler_OLED.slaveAddress                       		= OLED_ADDRESS;
	handler_OLED.mainClock							 		= MAIN_CLOCK_16_MHz_FOR_I2C;
	handler_OLED.maxI2C_FM							 		= I2C_MAX_RISE_TIME_FM_16MHz;
	handler_OLED.modeI2C_FM						 			= I2C_MODE_FM_SPEED_400KHz_16MHz;
	i2c_config(&handler_OLED);

	/* Configurar ADC */
	// Timer señal PWM
	handlerSignalPWM.ptrTIMx 								= TIM3;
	handlerSignalPWM.config.channel							= PWM_CHANNEL_1;
	handlerSignalPWM.config.duttyCicle						= 5;
	handlerSignalPWM.config.periodo							= 10000;
	handlerSignalPWM.config.prescaler						= 16;
	// Cargamos la configuración
	pwm_Config(&handlerSignalPWM);
	// Activamos la señal
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);


	// Cargando la configuracion para la conversion ADC
	adcConfig.canal[0]										= ADC_CHANNEL_0;
	adcConfig.canal[1]										= ADC_CHANNEL_1;
	adcConfig.resolution 									= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod[0] 							= ADC_SAMPLING_PERIOD_28_CYCLES;
	adcConfig.samplingPeriod[1] 							= ADC_SAMPLING_PERIOD_28_CYCLES
	adcConfig.dataAlignment      							= ADC_ALIGNMENT_RIGHT;
	adcConfig.quantity    									= 2;
	adc_Config(&adcConfig);


	/* Configuración PWM Zumbador */
	// PWM
	handlerPinPwmChannel3PC8.pGPIOx							= GPIOB;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.GPIO_PinNumber	= PIN_6;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_ALTFN;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel3PC8.GPIO_PinConfig.PinAltFunMode	= AF2;
	GPIO_Config(&handlerPinPwmChannel3PC8);

	// Timer señal PWM Timer3 Channel 3
	handlerSignalPWMPC8.ptrTIMx 							= 	TIM4;
	handlerSignalPWMPC8.config.channel						=	PWM_CHANNEL_1;
	handlerSignalPWMPC8.config.duttyCicle					= 	50;
	handlerSignalPWMPC8.config.periodo						= 	100;
	handlerSignalPWMPC8.config.prescaler					= 	16;
	// Cargamos la configuración
	pwm_Config(&handlerSignalPWMPC8);
	// Activamos la señal
	enableOutput(&handlerSignalPWMPC8);



}

void BasicTimer2_Callback(void){
	//Pin de estado
	GPIOxTooglePin(&handlerUserBlinkyPin);
}

//void usart1Rx_Callback(void){
//	//Interrupción por transmisión
//	rxData = getRxData();
//}

//void BasicTimer3_Callback(void){
//	muestreo = true;
//	contador++;
//}

void adcComplete_Callback(void){
	adcDataSingle = getADC();
	adcData[counterADC]= adcDataSingle;

	if(counterADC){
		if(adcData[1] > 4000 && paddleLocationB < 52){
			paddleLocationB++;
		}
		else if(adcData[1] < 100 && paddleLocationB > 0){
			paddleLocationB--;
		}
		counterADC = 0;
	}
	else{
		if(adcData[0] > 4000 && paddleLocationA < 52){
			paddleLocationA++;
		}
		else if(adcData[0] < 100 && paddleLocationA > 0){
			paddleLocationA--;
		}
		counterADC = 1;
	}

}


//void parseCommands(char *ptrBufferReception){
//
//	/* Lee la cadena de caracteres a la que apunta el "ptrBufferReception
//	 * y almacena en tres elementos diferentes: un string llamado "cmd",
//	 * y dos integer llamados "firstParameter" y "secondParameter"
//	 * De esta forma podemos introducir informacion al micro desde el puerto serial
//	 */
//	sscanf(ptrBufferReception, "%s %u %u %u %s", cmd, &firstParameter, &secondParameter, &thirdParameter, userMsg);
//	/* Comando help */
//	if (strcmp(cmd, "help") == 0){
//		writeMsg(&usart1Handler, "Help Menu CMDs: \n");
//		writeMsg(&usart1Handler, "1)  help 									-> Mostrar este menu \n");
//		writeMsg(&usart1Handler, "2)  MCOsignal [signal] 				-> Configurar signal 0:PLL 1:HSI 2:LSE\n");
//		writeMsg(&usart1Handler, "3)  MCOpres [prescaler] 				-> Configurar prescaler: 0,2,3,4,5 \n");
//		writeMsg(&usart1Handler, "4)  getDate 								-> Mostrar la hora y la fecha \n");
//		writeMsg(&usart1Handler, "5)  setDate [hora] [minuto] [segundo] 	-> Configurar la hora \n");
//		writeMsg(&usart1Handler, "6)  setTime [año] [mes] [día] 			-> Configurar la fecha \n");
//		writeMsg(&usart1Handler, "7)  setSummer 							-> Agregar una hora \n");
//		writeMsg(&usart1Handler, "8)  getADC 								-> Presentar los arreglos de datos ADC. \n");
//		writeMsg(&usart1Handler, "9)  setPeriod [periodo us] 				-> Configurar la velocidad de muestreo de la señal ADC\n");
//		writeMsg(&usart1Handler, "10) getData 								-> Adquirir datos acelerometro \n");
//		writeMsg(&usart1Handler, "11) getFrequency 							-> Frecuencia de los datos CMSIS-FFT \n");
//		writeMsg(&usart1Handler, "12) adjust [scaler] 						-> Sintonizar scaler: 1 - 31 \n");
//	}
//	/* Cambiar la configuración de la señal  del MCO1 */
//	else if (strcmp(cmd, "MCOsignal") == 0) {
//		if(firstParameter == 0 || firstParameter == 1 || firstParameter == 2){
//			selectMCOSignal(firstParameter);
//			writeMsg(&usart1Handler, "Signal configured");
//		}
//		else{
//			writeMsg(&usart1Handler, "Parametro no valido\n");
//		}
//	}
//
//	else if (strcmp(cmd, "MCOpres") == 0) {
//		if(firstParameter == 0 || firstParameter == 2 || firstParameter == 3 || firstParameter == 4 || firstParameter == 5){
//			selectMCOpresc(firstParameter);
//			stopPwmSignal(&handlerSignalPWM);	sprintf(bufferData, "Prescaler configured: %u", firstParameter);
//			writeMsg(&usart1Handler, bufferData);
//		}
//		else{
//			writeMsg(&usart1Handler, "Parametro no valido\n");
//		}
//	}
//
//	/* Almacenar datos ADC en arreglos de 256 datos */
//	else if (strcmp(cmd, "getADC") == 0) {
//		startPwmSignal(&handlerSignalPWM);
//
//		if(adcIsComplete){
//			stopPwmSignal(&handlerSignalPWM);
//			for (int i=0; i<256; i++){
//				sprintf(bufferData, "%u ; %u \n", (unsigned int )datosADC[0][i],(unsigned int )datosADC[1][i]);
//				writeMsg(&usart1Handler, bufferData);
//			}
//
//		}
//	}
//	/* Cambiar Velocidad de muestreo */
//	else if (strcmp(cmd, "setPeriod") == 0) {
//		if(firstParameter >0){
//			updateFrequency(&handlerSignalPWM, firstParameter);
//			updateDuttyCycle(&handlerSignalPWM, firstParameter / 2);
//			sprintf(bufferData, "set Periodo %u microsegundos", firstParameter);
//			writeMsg(&usart1Handler, bufferData);
//		}
//		else{
//			writeMsg(&usart1Handler, "Periodo no valido\n");
//		}
//	}
//	/* Mostrar la hora y la fecha */
//	else if (strcmp(cmd, "getDate") == 0) {
//		read_date(bufferData);
//		writeMsg(&usart1Handler, bufferData);
//	}
//
//	/* Configurar la hora	 */
//	else if (strcmp(cmd, "setTime") == 0) {
//		if(firstParameter >=0 && firstParameter < 24 && secondParameter >= 0
//				&& secondParameter < 60 && thirdParameter >=0 && thirdParameter < 60){
//			handler_RTC.config.hour 	= firstParameter;
//			handler_RTC.config.minutes = secondParameter;
//			handler_RTC.config.seconds = thirdParameter;
//			handler_RTC.config.date 	= getDay();
//			handler_RTC.config.month 	= getMonth();
//			handler_RTC.config.year		= getYear();
//
//			config_RTC(&handler_RTC);
//
//			read_date(bufferData);
//			writeMsg(&usart1Handler, bufferData);
//		}
//		else{
//			writeMsg(&usart1Handler, "Time no valido\n");
//		}
//	}
//	/* Configurar la fecha */
//	else if (strcmp(cmd, "setDate") == 0) {
//		if(firstParameter > 0 && firstParameter < 32 && secondParameter > 0
//				&& secondParameter < 13 && thirdParameter >=0 && thirdParameter < 100){
//			handler_RTC.config.hour 	= getHour();
//			handler_RTC.config.minutes = getMinutes();
//			handler_RTC.config.seconds = getSeconds();
//			handler_RTC.config.date 	= firstParameter;
//			handler_RTC.config.month 	= secondParameter;
//			handler_RTC.config.year		= thirdParameter;
//			config_RTC(&handler_RTC);
//			read_date(bufferData);
//			writeMsg(&usart1Handler, bufferData);
//		}
//		else{
//			writeMsg(&usart1Handler, "Date no valido\n");
//		}
//	}
//	/*Add 1 hour (summer time change) */
//	else if (strcmp(cmd, "setSummer") == 0) {
//		summerTime();
//		delay_ms(50);
//		read_date(bufferData);
//		writeMsg(&usart1Handler, bufferData);
//
//	}
//
//	/* Lanzar la captura de datos del acelerometro */
//	else if (strcmp(cmd, "getData") == 0) {
//		sprintf(bufferData, "Adquiriendo datos ... \n");
//		writeMsg(&usart1Handler, bufferData);
//		//i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x08);
//		contador = 0;
//		muestreo = true;
//		while(contador < 128){
//			if(muestreo){
//				i2c_readMultipleRegister(&Accelerometer, ACCEL_XOUT_L, measure);
//				AccelX_low 	= measure[0];
//				AccelX_high = measure[1];
//				AccelY_low 	= measure[2];
//				AccelY_high = measure[3];
//				AccelZ_low 	= measure[4];
//				AccelZ_high = measure[5];
//
//				AccelX = AccelX_high << 8 | AccelX_low;
//				AccelY = AccelY_high << 8 | AccelY_low;
//				AccelZ = AccelZ_high << 8 | AccelZ_low;
//
//				datosAcelerometroX[contador] 	= AccelX;
//				datosAcelerometroY[contador] 	= AccelY;
//				datosAcelerometroZ[contador]	= AccelZ;
//
//				muestreo = false;
//			}
//		}
//
//		contador = 0;
//
//		//Inicialización FFT
//		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fttSize);
//
//		if(statusInitFFT == ARM_MATH_SUCCESS){
//			//Imprimir mensaje de exito
//			sprintf(bufferData, "Datos adquiridos \n");
//			writeMsg(&usart1Handler, bufferData);
//		}
//
//		// Imprimir el areglo de 128 datos
//		for(int i = 0; i < 128; i++){
//			sprintf(bufferData, "n=%d x=%4.2f y=%4.2f z=%4.2f\n", i, datosAcelerometroX[i] * 9.8 / 256, datosAcelerometroY[i]*9.8 / 256, datosAcelerometroZ[i]*9.8 / 256);
//			writeMsg(&usart1Handler, bufferData);
//		}
//	}
//
//	/*  Presentar los datos de la frecuencia leída por el acelerómetro (CMSIS-FFT) */
//	else if (strcmp(cmd, "getFrequency") == 0) {
//		sprintf(bufferData, "getFrecuency  \n");
//		writeMsg(&usart1Handler, bufferData);
//		stopTime = 0.0;
//
//		int j = 0;
//
//		sprintf(bufferData, "FFT \n");
//		writeMsg(&usart1Handler, bufferData);
//		//Calcular la transformada de Fourier
//		if(statusInitFFT == ARM_MATH_SUCCESS){
//			arm_rfft_fast_f32(&config_Rfft_fast_f32, datosAcelerometroY, transformedSignalY, ifftFlag);
//			arm_abs_f32(transformedSignalY, datosAcelerometroY, fttSize);
//
//			// Hallar el máximo e imprimir datos en el dominio de las frecuencias
//			uint32_t indexMax = 0;
//			float FTT_Max = datosAcelerometroY[1];
//			for(int i = 1; i < fttSize; i++){
//				if(i%2){
//					if(datosAcelerometroY[i] > FTT_Max){
//						FTT_Max = datosAcelerometroY[i];
//						indexMax = j;
//					}
//					sprintf(bufferData, "%u ; %#.6f\n", j, 2*datosAcelerometroY[i]);
//					writeMsg(&usart1Handler, bufferData);
//					j++;
//				}
//			}
//
////			sprintf(bufferData, "index %d ; result %f\n,", (int)indexMax, FTT_Max);
////			writeMsg(&usart1Handler, bufferData);
//
//			// Calcular la frecuencia
//			float w = (indexMax) / ((fttSize) * 0.005);
//			sprintf(bufferData, "frecuency w %f Hz \n,", w);
//			writeMsg(&usart1Handler, bufferData);
//
//		}
//		else{
//			writeMsg(&usart1Handler, "FFT not initialized... \n");
//		}
//	}
//
//
//	else if (strcmp(cmd, "adjust") == 0) {
//			if(firstParameter > 0 && firstParameter < 32){
//				RCC->CR &= ~(RCC_CR_HSITRIM_Msk);					// Limpiar
//				RCC->CR |= (firstParameter<<RCC_CR_HSITRIM_Pos); 	// Calibrar reloj
//				writeMsg(&usart1Handler, "Adjust\n");
//			}
//			else{
//				writeMsg(&usart1Handler, "Parametro invalido\n");
//			}
//		}
//
//	else {
//		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
//		sprintf(bufferData, "Wrong CMD \n");
//		writeMsg(&usart1Handler, bufferData);
//		writeMsg(&usart1Handler, cmd);
//		writeMsg(&usart1Handler, "\n");
//
//	}
//
//	firstParameter  = 0;
//	secondParameter = 1000;
//	secondParameter = 1000;
//
//}

void draw(void){
	ClearScreen();

	//dibujar paleta A
	DrawBoxFilled(PADDLE_PADDING, paddleLocationA, PADDLE_PADDING + PADDLE_WIDTH, paddleLocationA + PADDLE_HEIGHT, 1);
	//dibujar paleta B
	DrawBoxFilled(SCREEN_WIDTH- PADDLE_WIDTH - PADDLE_PADDING, paddleLocationB, SCREEN_WIDTH - PADDLE_PADDING , paddleLocationB + PADDLE_HEIGHT, 1);


	for(int i = 0; i < SCREEN_HEIGHT; i+=4){
		DrawLineVertical(SCREEN_WIDTH/2, i, i + 2, 1);
	}

  //dibujar pelota
  DrawBoxFilled(ballX, ballY, ballX + BALL_SIZE, ballY + BALL_SIZE, 1);


  //Dibujar puntajes
  DrawChar(scoreA + '0', SCREEN_WIDTH/2 - SCORE_PADDING - 6, SCORE_PADDING, 1);
  DrawChar(scoreB + '0', SCREEN_WIDTH/2 + SCORE_PADDING, SCORE_PADDING, 1);

}

void ball(void){
	 ballX += ballSpeedX;
	 ballY += ballSpeedY;

	paddleSpeedA = paddleLocationA - lastPaddleLocationA;
	paddleSpeedB = paddleLocationB - lastPaddleLocationB;

	lastPaddleLocationA = paddleLocationA;
	lastPaddleLocationB = paddleLocationB;

	 //rebote superior e inferior
   if (ballY >= SCREEN_HEIGHT - BALL_SIZE || ballY <= 0) {
	 ballSpeedY *= -1;
	 soundBounce();
   }

   //rebote paleta A
   if (ballX >= PADDLE_PADDING && ballX <= PADDLE_PADDING+BALL_SIZE && ballSpeedX < 0) {
	   if (ballY > paddleLocationA - BALL_SIZE && ballY < paddleLocationA + PADDLE_HEIGHT) {
		   soundBounce();
		   ballSpeedX *= -1;
		   addEffect(paddleSpeedA);
	   }
   }

   //rebote paleta B
   if (ballX >= SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING-BALL_SIZE && ballX <= SCREEN_WIDTH-PADDLE_PADDING-BALL_SIZE && ballSpeedX > 0) {
       if (ballY > paddleLocationB - BALL_SIZE && ballY < paddleLocationB + PADDLE_HEIGHT) {
         soundBounce();
         ballSpeedX *= -1;

         addEffect(paddleSpeedB);
       }
   }

   //anotacion de puntos cuando se escapa de las paletas
   if (ballX >= SCREEN_WIDTH - BALL_SIZE || ballX <= 0) {
	   	  if (ballSpeedX > 0) {
	   		  scoreA++;
	   		  soundPoint();
	   		  ballX = SCREEN_WIDTH / 4;
	   	  }
	   	  if (ballSpeedX < 0) {
	   		  scoreB++;
	   		  soundPoint();
	   		  ballX = SCREEN_WIDTH / 4 * 3;
	   	  }


   }
}

//efecto de la pelota
void addEffect(int paddleSpeed){
	float oldBallSpeedY = ballSpeedY;

	//añadir efecto a la bola cuando paleta se mueve mientras rebota .
	//para cada píxel de movimiento de la paleta , añadir o quitar velocidad en EFFECT_SPEED
	for (int effect = 0; effect < abs(paddleSpeed); effect++) {
		if (paddleSpeed > 0) {
			ballSpeedY += EFFECT_SPEED;
		} else {
			ballSpeedY -= EFFECT_SPEED;
		}
	}

	//limite de velocidad minima
	if (ballSpeedY < MIN_Y_SPEED && ballSpeedY > -MIN_Y_SPEED) {
		if (ballSpeedY > 0) ballSpeedY = MIN_Y_SPEED;
		if (ballSpeedY < 0) ballSpeedY = -MIN_Y_SPEED;
		if (ballSpeedY == 0) ballSpeedY = oldBallSpeedY;
	}

	//limite de velocidad maxima
	if (ballSpeedY > MAX_Y_SPEED) ballSpeedY = MAX_Y_SPEED;
	if (ballSpeedY < -MAX_Y_SPEED) ballSpeedY = -MAX_Y_SPEED;
}

//sonido rebote
void soundBounce(void) {
	handlerSignalPWMPC8.config.periodo = 100;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(2);
	stopPwmSignal(&handlerSignalPWMPC8);

}

void soundPoint(void){
	handlerSignalPWMPC8.config.periodo = 500;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);
	stopPwmSignal(&handlerSignalPWMPC8);
}

void soundStart(void) {
	handlerSignalPWMPC8.config.periodo = 800;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);
	stopPwmSignal(&handlerSignalPWMPC8);
	handlerSignalPWMPC8.config.periodo = 400;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);
	stopPwmSignal(&handlerSignalPWMPC8);
	handlerSignalPWMPC8.config.periodo = 200;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);
	stopPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);

}

