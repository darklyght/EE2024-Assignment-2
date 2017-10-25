/*
 * cats_sw3.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_sw3.h"

void sw3_init(void) {
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 1;				// Set SW3 to EINT0 function
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<10), 0);

	LPC_SC->EXTMODE |= (1<<0);		// Set mode to edge detection
	LPC_SC->EXTPOLAR &= ~(1<<0);	// Set falling edge detection
	NVIC_EnableIRQ(EINT0_IRQn);
}

void sw3_timer_init(void) {
	LPC_SC->PCONP |= (1<<1);		// Power up Timer0
	LPC_SC->PCLKSEL0 |= (1<<2);		// CCLK
	LPC_TIM0->MR0 = 0x05F5E100;		// Match0 1s
	LPC_TIM0->MCR |= (1<<0);		// Interrupt on Match0
	LPC_TIM0->MCR |= (1<<1);		// Reset on Match0
	LPC_TIM0->MCR |= (1<<2);		// Stop on Match0
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void sw3_timer_start(void) {
	LPC_TIM0->TCR |= (1<<1);		// Reset Timer0
	LPC_TIM0->TCR &= ~(1<<1);		// Clear reset
	LPC_TIM0->TCR |= (1<<0);		// Start timer
}
