/*
 * cats_mode.h
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#ifndef CATS_MODE_H_
#define CATS_MODE_H_

#include "header.h"

void to_mode_stationary(STATE* catsState);
void to_mode_forward(STATE* catsState, TICKS* catsTicks, ACC* catsAcc, TEMP_VARS* catsTemp, uint8_t* ACCX_DISPLAY, uint8_t* TEMP_DISPLAY, uint8_t* UART_DISPLAY);
void to_mode_reverse(STATE* catsState, TICKS* catsTicks, uint8_t* UART_DISPLAY);
void in_mode_stationary(STATE* catsState);
void in_mode_forward(STATE* catsState, TICKS* catsTicks, ACC* catsAcc, TEMP_VARS* catsTemp, uint8_t* ACCX_DISPLAY, uint8_t* TEMP_DISPLAY, uint8_t* UART_DISPLAY);
void in_mode_reverse(STATE* catsState, TICKS* catsTicks, uint16_t* luxVal, SPEAKER* catsSpeaker, uint8_t* UART_DISPLAY);

#endif /* CATS_MODE_H_ */
