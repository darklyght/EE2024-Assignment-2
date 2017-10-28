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
