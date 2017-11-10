/*
 * cats_oled.c
 *
 *  Created on: Oct 23, 2017
 *      Author: darklyght
 */

#include "cats_oled.h"

/******************************************************************************//*
 * @brief 		Generate OLED display for temperature
 * @param[in]	tempDisplay is the string to print to
 * @param[in]	temp is the temperature as measured from temperature sensor
 * @return 		None
 *******************************************************************************/
void temp_display_gen(uint8_t* tempDisplay, int32_t temp) {
	char s[16] = "";
	strcpy((char*)tempDisplay, "");
	sprintf(s, "%.2f", temp/10.0);
	strcat((char*)tempDisplay, s);
}

/******************************************************************************//*
 * @brief 		Generate OLED display for acceleration
 * @param[in]	accDisplay is the string to print to
 * @param[in]	acc is the acceleration as measured from accelerometer
 * @return 		None
 *******************************************************************************/
void acc_display_gen(uint8_t* accDisplay, float acc) {
	char s[16] = "";
	strcpy((char*)accDisplay, "");
	sprintf(s, "%.2f", acc);
	strcat((char*)accDisplay, s);
}

/******************************************************************************//*
 * @brief 		Display temperature on OLED
 * @param[in]	tempDisplay is the string to print to
 * @param[in]	temp is the temperature as measured from temperature sensor
 * @return 		None
 *******************************************************************************/
void temp_display(uint8_t* tempDisplay, int32_t temp) {
	oled_putString(60, 10, (uint8_t*)"     ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	temp_display_gen(tempDisplay, temp);
	oled_putString(60, 10, tempDisplay, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
}

/******************************************************************************//*
 * @brief 		Display acceleration on OLED
 * @param[in]	accDisplay is the string to print to
 * @param[in]	acc is the acceleration as measured from accelerometer
 * @return 		None
 *******************************************************************************/
void acc_display(uint8_t* accDisplay, float acc) {
	oled_putString(60, 20, (uint8_t*)"     ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	acc_display_gen(accDisplay, acc);
	oled_putString(60, 20, accDisplay, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
}
