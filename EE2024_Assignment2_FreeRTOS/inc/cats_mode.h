/*
 * cats_mode.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_MODE_H_
#define CATS_MODE_H_

#include "cats_header.h"
#include "cats_timer.h"
#include "cats_temp.h"
#include "cats_acc.h"
#include "cats_light.h"
#include "cats_sseg.h"
#include "cats_rgb.h"
#include "cats_oled.h"
#include "cats_amp.h"
#include "cats_uart.h"

void to_mode_stationary(STATE* state, TICKS* ticks);
void to_mode_forward(STATE* state, TICKS* ticks, TEMP* temp, DATA* data, DISPLAY* display);
void to_mode_reverse(STATE* state, TICKS* ticks, DISPLAY* display);
void in_mode_stationary(void);
void in_mode_forward(STATE* state, TICKS* ticks, TEMP* temp, DATA* data, DISPLAY* display);
void in_mode_reverse(STATE* state, DATA* data, AMP* amp, DISPLAY* display);

#endif /* CATS_MODE_H_ */
