/*
 * cats_helper.h
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#ifndef CATS_HELPER_H_
#define CATS_HELPER_H_

#include "header.h"

void rgb_set(uint8_t ledMask);
void rgb_blink(STATE* catsState);
void acc_display_gen(uint8_t* ACCX_DISPLAY, int8_t accX);
void temp_display_gen(uint8_t* TEMP_DISPLAY, int32_t temp);
void set_uart_message(uint8_t* UART_DISPLAY, char* string);
void uart_display_gen(char* UART_MESSAGE, uint32_t messageCounter, int8_t accX, int32_t temp);
uint16_t obstacle_led_gen(uint16_t light);
uint32_t obstacle_speaker_gen(uint16_t light);

#endif /* CATS_HELPER_H_ */
