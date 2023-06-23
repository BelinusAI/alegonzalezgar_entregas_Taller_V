#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"
#include "AdcDriver.h"
#include "PwmDriver.h"
#include "stdbool.h"
#include "stdlib.h"
#include "ExtiDriver.h"

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

#define ONEPLAYER			34
#define TWOPLAYER			46



/*Definición de elementos*/
//Led de estado
GPIO_Handler_t handlerUserBlinkyPin 		= {0};
BasicTimer_Handler_t handlerBlinkyTimer2	= {0};

//Buttom
GPIO_Handler_t handlerUserButtom  			= {0};
EXTI_Config_t handlerUserButtomExti 		= {0};

GPIO_Handler_t handlerUserButtom2  			= {0};
EXTI_Config_t handlerUserButtomExti2 		= {0};

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

/* PONG */
int paddleLocationA			= 34;
int paddleLocationB 		= 46;
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
int selection 				= 34;
bool pause 					= false;
bool enterMenu				= true;
bool multiplayer 			= true;

//Prototipos de funciones
void init_Hadware(void);
void draw(void);
void ball(void);
void soundBounce(void);
void soundPoint(void);
void soundStart(void);
void addEffect(int paddleSpeed);
void showMenu(void);


int main (void){
	/*Activamos el coprocesador matemático FPU */
	SCB->CPACR |= (0xF <<20);

	/* Inicio de sistema */
	init_Hadware();
	OLED_Init(&handler_OLED);
	startScreen();
	UpdateDisplay(&handler_OLED);
	delay_ms(3000);
	soundStart();

	while(1){
		/* Menú de inicio */
		while(enterMenu){
			showMenu();
			/* Movimiento del cursor en el menu */
			if(!counterADC){
				if(adcData[counterADC] > 4000){
					if(selection == ONEPLAYER){
						selection = TWOPLAYER;
						soundBounce();
					}
				}
				else if(adcData[counterADC] < 100){
					if(selection == TWOPLAYER){
						selection = ONEPLAYER;
						soundBounce();
					}
				}
				counterADC = 1;
			}
			else{
				counterADC = 0;
			}
			// Inicio del juego
			if(!enterMenu){
				delay_ms(200);
				soundStart();
				draw();
				UpdateDisplay(&handler_OLED);
				counterADC = 1;
			}
		}

		/* Game */
		ball();
		draw();
		UpdateDisplay(&handler_OLED);


		/* Condición de victoria */
		 if (scoreA >= 6){ //gana jugador A al anotar 6 puntos
			 DrawString("Player1 Win!", 32, 30, 1);
			 UpdateDisplay(&handler_OLED);
			 delay_ms(3000);
			 startScreen();
			 UpdateDisplay(&handler_OLED);
			 delay_ms(3000);
			 enterMenu = true;
			 scoreA = 0;
			 scoreB = 0;
		 }else if (scoreB >= 6){ //gana jugador B al anotar 6 puntos
			 DrawString("Player2 Win!", 32, 30, 1);
			 UpdateDisplay(&handler_OLED);
			 delay_ms(3000);
			 startScreen();
			 UpdateDisplay(&handler_OLED);
			 delay_ms(3000);
			 enterMenu = true;
			 scoreA = 0;
			 scoreB = 0;
		 }

		 /* Pause */
		 if(pause){
			 DrawString("PAUSE", 48, 30, 1);
			 UpdateDisplay(&handler_OLED);
			 while(pause){
				 __NOP();
			 }
		 }
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

	//Puerto PC8 UserButtom
	handlerUserButtom.pGPIOx 								= GPIOC;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerUserButtom.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButtom.GPIO_PinConfig.PinPuPdControl			= GPIO_PUPDR_PULLUP;
	GPIO_Config(&handlerUserButtom);

	//EXTI
	handlerUserButtomExti.pGPIOHandler 						= &handlerUserButtom;
	handlerUserButtomExti.edgeType 							= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtomExti);

	//Puerto PA10 UserButtom
	handlerUserButtom2.pGPIOx 								= GPIOA;
	handlerUserButtom2.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerUserButtom2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButtom2.GPIO_PinConfig.PinPuPdControl		= GPIO_PUPDR_PULLUP;
	GPIO_Config(&handlerUserButtom2);

	//EXTI
	handlerUserButtomExti.pGPIOHandler 						= &handlerUserButtom2;
	handlerUserButtomExti.edgeType 							= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtomExti);

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

// Play and Pause
void callback_extInt10(void){
	if(enterMenu && selection == TWOPLAYER){
		enterMenu = false;
	}
	else if(enterMenu && selection == ONEPLAYER){
		enterMenu = false;
		multiplayer = false;
	}
	else{
		pause = !pause;
	}
}
// Play and Pause
void callback_extInt8(void){
	if(enterMenu && selection == TWOPLAYER){
		enterMenu = false;
	}
	else if(enterMenu && selection == ONEPLAYER){
		enterMenu = false;
		multiplayer = false;
	}
	else{
		pause = !pause;
	}

}

//Lectura del Joystyck
void adcComplete_Callback(void){
	adcDataSingle = getADC();
	adcData[counterADC]= adcDataSingle;

	if(!enterMenu){
		if(counterADC){ // Player2
			if(multiplayer){ //Player1 Manual
				if(adcData[counterADC] < 100 && paddleLocationB < 52){
					paddleLocationB++;
				}
				else if(adcData[counterADC] > 4000 && paddleLocationB > 0){
					paddleLocationB--;
				}
			}
			// Player 2 automatico
			else{
				if(ballX <= 96 && ballY < 57 && ballY > 4){
					paddleLocationB = ballY - 4;
				}
				else if(ballX > 96 && ballSpeedX < 0 && ballY < 57 && ballY > 4){
					paddleLocationB = ballY - 4;
				}
				else if(ballX > 96 && ballX < 112 && ballSpeedX > 0 && paddleLocationB < 52 && paddleLocationB > 0){
					if(ballSpeedY > 0){
						if(!(rand() % 5)){
							paddleLocationB++;
						}
					}else{
						if(!(rand() % 5)){
							paddleLocationB--;
						}
					}
				}
			}
			counterADC = 0;

		}else{ //Player 1
			if(adcData[counterADC] > 4000 && paddleLocationA < 52){
				paddleLocationA++;
			}
			else if(adcData[counterADC] < 100 && paddleLocationA > 0){
				paddleLocationA--;
			}
			counterADC = 1;
		}
	}
}

// Dibujas escenario
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

// Movimiento de la bola
void ball(void){
	 ballX += ballSpeedX;
	 ballY += ballSpeedY;

	paddleSpeedA = paddleLocationA - lastPaddleLocationA;
	paddleSpeedB = paddleLocationB - lastPaddleLocationB;

	lastPaddleLocationA = paddleLocationA;
	lastPaddleLocationB = paddleLocationB;

	 //rebote superior e inferior
	if(ballY >= SCREEN_HEIGHT - BALL_SIZE || ballY <= 0) {
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
		  ballSpeedX = 2;

	  }
	  if (ballSpeedX < 0) {
		  scoreB++;
		  soundPoint();
		  ballX = SCREEN_WIDTH / 4 * 3;
		  ballSpeedX = -2;
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

// Sonido de puntuación
void soundPoint(void){
	handlerSignalPWMPC8.config.periodo = 500;
	setFrequency(&handlerSignalPWMPC8);
	startPwmSignal(&handlerSignalPWMPC8);
	delay_ms(100);
	stopPwmSignal(&handlerSignalPWMPC8);
}

// Sonido de inicio
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

// Presentar Menú
void showMenu(void){
	startMenu();
	//dibujar paleta A
	DrawBoxFilled(PADDLE_PADDING, ONEPLAYER, PADDLE_PADDING + PADDLE_WIDTH, ONEPLAYER + PADDLE_HEIGHT, 1);
	//dibujar paleta B
	DrawBoxFilled(SCREEN_WIDTH- PADDLE_WIDTH - PADDLE_PADDING, TWOPLAYER, SCREEN_WIDTH - PADDLE_PADDING , TWOPLAYER + PADDLE_HEIGHT, 1);
	DrawString("1 player", 44, 33, 1);
	DrawString("2 player", 44, 44, 1);
	DrawBoxFilled(38, selection, 38 + BALL_SIZE, selection + BALL_SIZE, 1);

	UpdateDisplay(&handler_OLED);
}

