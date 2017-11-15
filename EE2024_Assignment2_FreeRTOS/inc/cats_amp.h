/*
 * cats_amp.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_AMP_H_
#define CATS_AMP_H_

#include "cats_header.h"
#include "lpc17xx_pwm.h"

void amp_init(void);
void amp_toggle(AMP* amp);
void amp_start(void);
void amp_stop(void);
void amp_beep_mode(void);
void amp_music_mode(void);
void pwm_low(void);
void pwm_high(void);
void amp_volume(uint8_t rotary, uint8_t* ampVolume);

#endif /* CATS_AMP_H_ */
