/*
 * cats_led.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_rgb.h"

/******************************************************************************//*
 * @brief 		Initialise GPIO for RGB
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void rgb_init(void) {
	GPIO_SetDir(2, 1, 1);		// Red
	GPIO_SetDir(2, (1<<8), 1);	// Blue
}

/******************************************************************************//*
 * @brief 		Set RGB
 * @param[in]	mask is the colours to turn on
 * @return 		None
 *******************************************************************************/
void rgb_set(uint8_t mask) {
	if ((mask & RGB_RED) != 0) {
		GPIO_SetValue(2, 1);
	} else {
		GPIO_ClearValue(2, 1);
	}
	if ((mask & RGB_BLUE) != 0) {
		GPIO_SetValue(2, (1<<8));
	} else {
		GPIO_ClearValue(2, (1<<8));
	}
}

/******************************************************************************//*
 * @brief 		Blink the RGB
 * @param[in]	state is the pointer to the state of the device
 * @return 		None
 *******************************************************************************/
void rgb_blink(STATE* state) {
	if (state->accState == ACC_HIGH && state->tempState == TEMP_HIGH) {
		if (GPIO_ReadValue(2)>>0 & 0x1) {
			rgb_set(RGB_OFF);
		} else {
			rgb_set(RGB_BLUE | RGB_RED);
		}
	} else if (state->accState == ACC_HIGH) {
		if (GPIO_ReadValue(2)>>8 & 0x1) {
			rgb_set(RGB_OFF);
		} else {
			rgb_set(RGB_BLUE);
		}
	} else if (state->tempState == TEMP_HIGH) {
		if (GPIO_ReadValue(2)>>0 & 0x1) {
			rgb_set(RGB_OFF);
		} else {
			rgb_set(RGB_RED);
		}
	}
}
