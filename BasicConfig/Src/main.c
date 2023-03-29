/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

//Definición de elementos


//Prototipos de funciones
void init_Hadware(void);


int main(void){
	init_Hadware();

	while(1){

	}

	return 0;

}

void init_Hadware(void){

}

