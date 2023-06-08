/*
 * RtcDriver.c
 *
 *  Created on: 7/06/2023
 *  Author: alegonzalezgar
 */

#include "RtcDriver.h"



void config_RTC(RTC_Handler_t *ptrHandlerRTC){
	// Activamos la señal de reloj para el PWR
	RCC -> APB1ENR |= RCC_APB1ENR_PWREN;

	// Desactivamos el modo Backup Protection
	PWR -> CR |= PWR_CR_DBP;

	// HSelect the RTC clock source LSE (LOW SPEED EXTERNAL 32KHz)
	RCC ->BDCR |= RCC_BDCR_RTCSEL_0;

	// Enable the RTC clock
	RCC -> BDCR |= RCC_BDCR_RTCEN;

	// Activamos el LSE (LOW SPEED EXTERNAL 32KHz)
	RCC -> BDCR |= RCC_BDCR_LSEON;
	/* Wait until LSE is ready */
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP() ;
	}


	/* Iniciamos rutina de inicialización */

	// Desactivamos el Write Protection register con la clave
	RTC -> WPR = 0xCAU;
	RTC -> WPR = 0x53U;

	/* 1.0 Setiamos el bit INIT	en el ISR */
	RTC -> ISR |= RTC_ISR_INIT;

	/* 2.0 Ponemos en 1 el bit de INITF */
	while((RTC->ISR & RTC_ISR_INITF)==0){
		__NOP();
	}

	/* 3.0 Configuramos el preescaler */

	RTC -> PRER |= RTC_PRER_PREDIV_A;
	RTC -> PRER |= (0xFF << RTC_PRER_PREDIV_S_Pos);

	//RTC->CR |= RTC_CR_BYPSHAD;

	// 4 Load the initial time and date values in the shadow registers and configure the time format
	//RTC->CR |= RTC_CR_BYPSHAD;

	RTC->TR = 0;
	RTC->DR = 0;

	// Escogemos el modo de 24 horas
	RTC -> CR |= ~(RTC_CR_FMT); //Hour format 24 hour/day format

	// RTC clock enable
	RTC->DR |= ((ptrHandlerRTC->config.year / 10) << RTC_DR_YT_Pos);  // Year Tens in BCD
	RTC->DR |= ((ptrHandlerRTC->config.year % 10) << RTC_DR_YU_Pos);  // Year Units in BCD

	RTC->DR &=  ~RTC_DR_WDU;  // WeekDay in BCD

	RTC->DR |= ((ptrHandlerRTC->config.month / 10) << RTC_DR_MT_Pos);  // Month Units in BCD
	RTC->DR |= ((ptrHandlerRTC->config.month % 10) << RTC_DR_MU_Pos);  // Month Tens in BCD

	RTC->DR |= ((ptrHandlerRTC->config.date / 10) << RTC_DR_DT_Pos);   // Date Tens in BCD
	RTC->DR |= ((ptrHandlerRTC->config.date % 10) << RTC_DR_DU_Pos);   // Date Units in BCD

	//RTC->CR |= ~(RTC_TR_PM);  	// 24 hours

	RTC->TR |= ((ptrHandlerRTC->config.hour / 10) << RTC_TR_HT_Pos);  // Hours Tens in BCD
	RTC->TR |= ((ptrHandlerRTC->config.hour % 10) << RTC_TR_HU_Pos);  // Hours Units in BCD

	RTC->TR |= ((ptrHandlerRTC->config.minutes / 10) << RTC_TR_MNT_Pos);  // Minutes Tens in BCD
	RTC->TR |= ((ptrHandlerRTC->config.minutes % 10) << RTC_TR_MNU_Pos);  // Minutes Units in BCD

	RTC->TR |= ((ptrHandlerRTC->config.seconds / 10) << RTC_TR_ST_Pos);   // Seconds Tens in BCD
	RTC->TR |= ((ptrHandlerRTC->config.seconds % 10) << RTC_TR_SU_Pos);   // Seconds Units in BCD


	/* 5 Exit the initialization mode by clearing the INIT bit. The actual calendar counter value is
	then automatically loaded and the counting restarts after 4 RTCCLK clock cycles. */
	RTC -> ISR &= ~RTC_ISR_INIT;

}

// Función para convertir de numeros decimales a código BCD
uint8_t decToBCD(int val){
	uint8_t variable= (uint8_t) ((val/10*16) + (val%10));
	return variable;
}

// Función para convertir de código BCD a numeros decimales
int BCDToDec(uint8_t val){
	int variable =(int) ((val/16*10) + (val%16));
	return variable;
}

void read_date(char *bufferDate){

	int RTC_Hours   = getHour();
	int RTC_Minutes = getMinutes();
	int RTC_Seconds = getSeconds();
	int RTC_year 	= getYear();
	int RTC_Month 	= getMonth();
	int RTC_Day		= getDay();

	sprintf(bufferDate, "%.2d:%.2d:%.2d - %.2d/%.2d/%.2d\n", RTC_Hours, RTC_Minutes, RTC_Seconds, RTC_Day, RTC_Month, RTC_year);

}

int getHour(void){
	int RTC_Hours   = ((RTC->TR & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos)  * 10 + ((RTC->TR & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
	return RTC_Hours;
}

int getMinutes(void){
	int RTC_Minutes = ((RTC->TR & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos) * 10 + ((RTC->TR & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
	return RTC_Minutes;
}

int getSeconds(void){
	int RTC_Seconds = ((RTC->TR & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos) * 10 + ((RTC->TR & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
	return RTC_Seconds;
}

int getYear(void){
	int RTC_year 	= ((RTC->DR & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos) * 10 + ((RTC->DR & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
	return RTC_year;
}

int getMonth(void){
	int RTC_Month 	= ((RTC->DR & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) * 10 + ((RTC->DR & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
	return RTC_Month;
}

int getDay(void){
	int RTC_Day		= ((RTC->DR & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos) * 10 + ((RTC->DR & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
	return RTC_Day;
}

void summerTime(void){
	RTC->CR |= RTC_CR_ADD1H;
}

