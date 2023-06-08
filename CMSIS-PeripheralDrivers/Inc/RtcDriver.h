/*
 * RtcDriver.h
 *
 *  Created on: 7/06/2023
 *  Author: alegonzalezgar
 *  */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>

typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t date;
	uint8_t month;
	uint16_t year;
}RTC_Config_t;

typedef struct
{
	RTC_TypeDef		*ptrRTC;	// Timer al que esta asociado el RTC
	RTC_Config_t	config;	// Configuración inicial del RTC
}RTC_Handler_t;


void config_RTC(RTC_Handler_t *ptrHandlerRTC);
uint8_t decToBCD(int val);
void read_date(char *bufferDate);
int getHour(void);
int getMinutes(void);
int getSeconds(void);
int getYear(void);
int getMonth(void);
int getDay(void);
void summerTime(void);

#endif /* RTCDRIVER_H_ */


