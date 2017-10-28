/*
 * cats_oled.c
 *
 *  Created on: Oct 23, 2017
 *      Author: darklyght
 */

#include "cats_oled.h"

void temp_display_gen(uint8_t* tempDisplay, int32_t temp) {
	char s[16] = "";
	strcpy((char*)tempDisplay, "");
	sprintf(s, "%.2f", temp/10.0);
	strcat((char*)tempDisplay, s);
}

void acc_display_gen(uint8_t* accDisplay, float acc) {
	char s[16] = "";
	strcpy((char*)accDisplay, "");
	sprintf(s, "%.2f", acc);
	strcat((char*)accDisplay, s);
}

void temp_display(uint8_t* tempDisplay, int32_t temp) {
	temp_display_gen(tempDisplay, temp);
	oled_putString(60, 10, (uint8_t*)"     ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(60, 10, tempDisplay, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
}

void acc_display(uint8_t* accDisplay, float acc) {
	acc_display_gen(accDisplay, acc);
	oled_putString(60, 20, (uint8_t*)"     ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(60, 20, accDisplay, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
}
