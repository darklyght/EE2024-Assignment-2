/*
 * cats_oled.h
 *
 *  Created on: Oct 23, 2017
 *      Author: darklyght
 */

#ifndef CATS_OLED_H_
#define CATS_OLED_H_

#include "cats_header.h"

void temp_display_gen(uint8_t* tempDisplay, int32_t temp);
void acc_display_gen(uint8_t* accDisplay, float acc);
void temp_display(uint8_t* tempDisplay, int32_t temp);
void acc_display(uint8_t* accDisplay, float acc);

#endif /* CATS_OLED_H_ */
