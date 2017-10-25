/*
 * cats_amp.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_amp.h"

void amp_init(void) {
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 25;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<24), 0);
	GPIO_SetDir(0, (1<<25), 0);

	GPIO_SetDir(2, 1<<6, 1);
	GPIO_SetDir(2, 1<<7, 1);
	GPIO_SetDir(2, 1<<13, 1);
	GPIO_SetDir(0, 1<<26, 1);

	LPC_GPIOINT->IO0IntEnF |= 1<<24;
	LPC_GPIOINT->IO0IntEnF |= 1<<25;

	LPC_SC->PCONP |= 1<<22;		// Power up Timer 2
	LPC_SC->PCLKSEL1 |= 1<<12;	// CCLK
	LPC_TIM2->MR0 = 1<<16;		// Suitable for LED
	LPC_TIM2->MCR |= 1<<0;		// Interrupt on Match0
	LPC_TIM2->MCR |= 1<<1;		// Reset timer on Match0
	LPC_TIM2->TCR |= 1<<1;		// Reset Timer2
	LPC_TIM2->TCR &= ~(1<<1);	// Reset timer
	LPC_TIM2->TCR &= ~(1<<0);	// Stop timer
	NVIC_EnableIRQ(TIMER2_IRQn);
}

void amp_beep(STATE* state, AMP* amp) {
	if (amp->pwmOn) {
		pwm_low();
		LPC_TIM2->TCR &= ~(1<<0);
		LPC_TIM2->IR |= 1<<0;
		amp->pwmOn = 0;
	} else {
		pwm_high();
		LPC_TIM2->TCR |= 1 << 0;
		LPC_TIM2->IR |= 1<<0;
		amp->pwmOn = 1;
	}
}

void amp_stop(void) {
	LPC_TIM2->TCR &= ~(1<<0);
}

void pwm_low(void) {
	GPIO_ClearValue(0, 1<<26);
}

void pwm_high(void) {
	GPIO_SetValue(0, 1<<26);
}

void amp_volume(uint8_t rotary, uint8_t* ampVolume) {
	if (rotary == 0x01) {
		GPIO_SetValue(2, 1<<7);
		*ampVolume = 0;
	} else if (rotary == 0x02) {
		GPIO_ClearValue(2, 1<<7);
		*ampVolume = 0;
	}
}

void amp_volume_clock(uint8_t* ampVolume) {
	if (*ampVolume >= 0 && *ampVolume < 3) {
		GPIO_ClearValue(2, 1<<6);
		(*ampVolume)++;
	} else if (*ampVolume >= 3 && *ampVolume < 6) {
		GPIO_SetValue(2, 1<<6);
		(*ampVolume)++;
	} else if (*ampVolume >= 6 && *ampVolume < 9) {
		GPIO_ClearValue(2, 1<<6);
		(*ampVolume)++;
	} else {
		*ampVolume = -1;
	}
}
