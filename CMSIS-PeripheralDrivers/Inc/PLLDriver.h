/*
 * PLLDriver.h
 *  Author: alegonzalezgar
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

#define FREQUENCY_80_MHz	0
#define FREQUENCY_100_MHz	1

void configPLL(void);
int getConfigPLL(void);

#endif /* PLLDRIVER_H_ */