/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Configuracion basica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */
#include <stdint.h>
#include <stm32f411xx_hal.h>

/**
 * Función principal del programa.
 **/
int main(void){

	//configuracion inicial del MCU
	RCC->AHB1ENR &= ~(1 << 0);
	RCC->AHB1ENR |= (1 << 0);

	//configurar el pin
	GPIOA->MODER &= (0b11 << 10);
	GPIOA->MODER |= (0b01 << 10);

	GPIOA->OTYPER &= (1 << 5);

	GPIOA->OSPEEDR &= (0b11 << 10);
	GPIOA->OSPEEDR |= (0b10 <<10);

	GPIOA->PUPDR &= (0b11 << 10);

	GPIOA->ODR &= (1 << 5);
	GPIOA->ODR |= (1 << 5);



    /* Loop forever */
	while(1){

	}

	return 0;
}
