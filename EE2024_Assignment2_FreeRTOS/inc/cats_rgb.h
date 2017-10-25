/*
 * cats_led.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_RGB_H_
#define CATS_RGB_H_

#define RGB_OFF   0x00
#define RGB_RED   0x01
#define RGB_BLUE  0x02
#define RGB_GREEN 0x04

#include "cats_header.h"

void rgb_init(void);
void rgb_set(uint8_t mask);
void rgb_blink(STATE* state);

#endif /* CATS_LED_H_ */
