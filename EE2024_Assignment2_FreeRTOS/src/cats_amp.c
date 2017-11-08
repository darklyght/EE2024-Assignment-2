/*
 * cats_amp.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_amp.h"

/******************************************************************************//*
 * @brief 		Initialise the amplifier
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_init(void) {
	PINSEL_CFG_Type PinCfg;

	// Initialise GPIO for amplifier
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(2, 1<<6, 1);
	GPIO_SetDir(2, 1<<7, 1);
	GPIO_SetDir(2, 1<<13, 1);
	GPIO_SetDir(0, 1<<26, 1);

	// Initialise GPIO for volume control
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 25;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<24), 0);
	GPIO_SetDir(0, (1<<25), 0);

	// Initialise rotary switch interrupt
	LPC_GPIOINT->IO0IntEnF |= 1<<24;
	LPC_GPIOINT->IO0IntEnF |= 1<<25;

	// Initialise Timer2 for PWM interrupt
	LPC_SC->PCONP |= 1<<22;		// Power up Timer2
	LPC_SC->PCLKSEL1 |= 1<<12;	// CCLK
	LPC_TIM2->MR0 = 1<<18;		// Suitable for LED
	LPC_TIM2->MCR |= 1<<0;		// Interrupt on Match0
	LPC_TIM2->MCR |= 1<<1;		// Reset timer on Match0
	LPC_TIM2->TCR |= 1<<1;		// Reset Timer2
	LPC_TIM2->TCR &= ~(1<<1);	// Clear Timer2 reset
	LPC_TIM2->TCR &= ~(1<<0);	// Stop timer
}

/******************************************************************************//*
 * @brief 		Alternate the amplifier on and off
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_beep(AMP* amp) {
	if (amp->pwmOn) {
		pwm_low(); 					// Set pin to low
		LPC_TIM2->IR |= 1<<0; 		// Clear Timer2 interrupt
		LPC_TIM2->TCR &= ~(1<<0); 	// Stop Timer2
		amp->pwmOn = 0;				// Set pwmOn flag to 0
	} else {
		pwm_high();					// Set pin to high
		LPC_TIM2->IR |= 1<<0;		// Clear Timer2 interrupt
		LPC_TIM2->TCR |= 1 << 0;	// Start Timer2
		amp->pwmOn = 1;				// Set pwmOn flag to 1
	}
}

/******************************************************************************//*
 * @brief 		Stop the PWM by stopping Timer2
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_stop(void) {
	LPC_TIM2->TCR &= ~(1<<0);
}

/******************************************************************************//*
 * @brief 		Set the PWM signal to low
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void pwm_low(void) {
	GPIO_ClearValue(0, 1<<26);
}

/******************************************************************************//*
 * @brief 		Set the PWM signal to high
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void pwm_high(void) {
	GPIO_SetValue(0, 1<<26);
}

/******************************************************************************//*
 * @brief 		Set the UP/DN of amplifier and set volume flag to 1
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_volume(uint8_t rotary, uint8_t* ampVolume) {
	if (rotary == 0x01) {
		GPIO_SetValue(2, 1<<7);
		*ampVolume = 1;
	} else if (rotary == 0x02) {
		GPIO_ClearValue(2, 1<<7);
		*ampVolume = 1;
	}
}
