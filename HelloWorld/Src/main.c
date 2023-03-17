/**
 ******************************************************************************
 * @file           : main.c
 * @author         : alegonzalezgar
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f411xx_hal.h>

/**
 * Función principal del programa
 * Encender LED2
*/
int main(void){

    //Activar reloj AHB1ENR bus
	RCC->AHB1ENR &= ~(1 << 0);
	RCC->AHB1ENR |= (1 << 0);

	//Pin AP5 GPIOA
	// GP(general-purpose) (PP)push-pull,
	GPIOA->MODER &= ~(0b11 << 10);
	GPIOA->MODER |= (0b01 << 10); //01

	GPIOA->OTYPER &= ~(1 << 5); //0

	GPIOA->OSPEEDR &= ~(0b11 <<10);
	GPIOA->OSPEEDR |= (0b10 << 10);

	GPIOA->PUPDR &= ~(0b11 << 10); //00

	GPIOA->ODR &= ~(1 << 5);
	GPIOA->ODR |= (1 << 5); //1

}
