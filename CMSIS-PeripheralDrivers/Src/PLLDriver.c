/*
 * PLLDriver.c
 * Author: alegonzalezgar
 */
#include "PLLDriver.h"

void configPLL(void){
	// HSI clock selected as PLL and PLLI2S clock entry
	RCC -> PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);

	/* Division factor for the main PLL (PLL) input clock
	 * VCO input frequency = PLL input clock frequency / PLLM with 2 ≤ PLLM ≤ 63
	 * VCO input frequency ranges from 1 to 2 MHz.*
	 */
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); // Limpiamos
	/*Ponemos PLLM en 8 */
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLM_3);

	/* Main PLL (PLL) multiplication factor for VCO
	* VCO output frequency = VCO input frequency × PLLN with 50 ≤ PLLN ≤ 432
	* VCO output frequency is between 100 and 432 MHz
	* */
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); // Limpiamos

	//PLLN = 100
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_2);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_5);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_6);

	/* Main PLL (PLL) division factor for main system clock
	 * PLL output clock frequency = VCO frequency / PLLP with PLLP = 2, 4, 6, or 8
	 * NOT to exceed 100 MHz on this domain
	 * */
	/* PLLP = 2 */
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);

	/* Latencia del FLASH_ACR */
	// 2 Wait states segun la tabla
	FLASH -> ACR &= ~(FLASH_ACR_LATENCY); // Limpiamos
	FLASH -> ACR |= (FLASH_ACR_LATENCY_3WS);

	// Encendemos el PLL
	RCC->CR |= RCC_CR_PLLON;

	// Esperamos que el PLL se cierre (estabilizacion)
	while (!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	// Cambiamos el CPU Clock source cambiando los SW bits (System Clock Switch)
	/* System Clock Switch para PLL */
	RCC -> CFGR &= ~(RCC_CFGR_SW); // Limpiamos
	RCC -> CFGR |= (RCC_CFGR_SW_1);

}

/* Funcion para leer el registro con la informacion de la Frecuencia del micro */
int getConfigPLL(void){
	uint32_t frequencyValue = 0;

	if(RCC->CFGR & RCC_CFGR_SWS_PLL){
		frequencyValue = 100000000;
	}else if(RCC->CFGR & RCC_CFGR_SWS_HSI){
		frequencyValue = 16000000;
	}
	return frequencyValue;
}

