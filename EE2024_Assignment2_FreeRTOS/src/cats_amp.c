/*
 * cats_amp.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_amp.h"
#include "sample_music.h"
#include "lpc17xx_rit.h"

//Number of ticks for 8kHz and 44.1kHz
#define LOW_FREQ 15000
#define HIGH_FREQ 2721

const RIT_CMP_VAL slowRitConfig = {LOW_FREQ, 0xFFFFFFFF, 0};
const RIT_CMP_VAL fastRitConfig = {HIGH_FREQ, 0xFFFFFFFF, 0};

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

	// Initialize PWM pins
	PinCfg.Funcnum = 1;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<1), 1);

	// Initialize PWM Controller
	PWM_TIMERCFG_Type pwmCfg;
	pwmCfg.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	pwmCfg.PrescaleValue = 1;
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &pwmCfg); //Initialize prescale
	PWM_Cmd(LPC_PWM1, ENABLE);
	PWM_CounterCmd(LPC_PWM1, ENABLE); //Start Counter
	PWM_MATCHCFG_Type matchConfig;
	matchConfig.MatchChannel = 0;
	matchConfig.ResetOnMatch = ENABLE;
	matchConfig.IntOnMatch = DISABLE;
	matchConfig.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &matchConfig); //Set TC reset on match 0

	// Initialize Repetitive Interrupt Timer
	RIT_Init(LPC_RIT);
	RIT_TimerClearCmd(LPC_RIT, ENABLE);
	RIT_TimerConfig(LPC_RIT, &slowRitConfig);
}

/******************************************************************************//*
 * @brief 		Toggles the amplifier on and off
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_toggle(AMP* amp) {
	if (amp->pwmOn) {
		amp_stop();
		amp->pwmOn = 0;
	} else {
		amp_start();
		amp->pwmOn = 1;
	}
}

void amp_beep_mode() {
	RIT_Cmd(LPC_RIT, DISABLE);
	PWM_MatchUpdate(LPC_PWM1, 0, 1<<18, PWM_MATCH_UPDATE_NEXT_RST); //Set PWM period time
	PWM_MatchUpdate(LPC_PWM1, 2, 1<<17, PWM_MATCH_UPDATE_NEXT_RST); //Set PWM active time
}

/******************************************************************************//*
 * @brief 		Start the PWM by enabling pwm1.2 output
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_start(void) {
	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE); //Enable pwm channel output
}

/******************************************************************************//*
 * @brief 		Stop the PWM by disabling pwm1.2 output
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void amp_stop(void) {
	PWM_ChannelCmd(LPC_PWM1, 2, DISABLE); //Disable pwm channel output
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

uint32_t playerCounter = 0;

void amp_music_mode() {
	playerCounter = 0;
	TIM_Cmd(LPC_TIM2, ENABLE);

	PWM_MatchUpdate(LPC_PWM1, 0, 256, PWM_MATCH_UPDATE_NEXT_RST); //Set PWM period time
	PWM_MatchUpdate(LPC_PWM1, 2, SAMPLE_MUSIC[0], PWM_MATCH_UPDATE_NEXT_RST); //Set PWM active time
	amp_start();
}

void RIT_IRQHandler(void) {
	playerCounter = (playerCounter + 1) % sizeof(SAMPLE_MUSIC);
	PWM_MatchUpdate(LPC_PWM1, 2, SAMPLE_MUSIC[playerCounter], PWM_MATCH_UPDATE_NEXT_RST); //Set PWM active time
	LPC_RIT->RICTRL |= 1<<0;
}
