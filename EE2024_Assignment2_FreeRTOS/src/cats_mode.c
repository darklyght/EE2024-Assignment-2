/*
 * cats_mode.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_mode.h"

uint8_t SEGMENT_DISPLAY[16] = "0123456789ABCDEF";

void to_mode_stationary(STATE* state, TICKS* ticks) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, (uint8_t*)"STATIONARY", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	sseg_set(0xFF, TRUE);
	rgb_set(0x00);
	vTaskSuspend(xRGBBlinkHandle);
	acc_interrupt_stop();
	acc_interrupt_clear();
//	lights_stop();
//	pca9532_setLeds (0x0000, 0xFFFF);
//	amp_stop();
	state->modeState = MODE_STATIONARY;
	state->accState = ACC_OFF;
	state->tempState = TEMP_OFF;
	state->lightState = LIGHT_OFF;
	ticks->x1sTicks = 0;
}

void to_mode_forward(STATE* state, TICKS* ticks, TEMP* temp, DATA* data, DISPLAY* display) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, (uint8_t*)"FORWARD", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(0, 10, (uint8_t*)"TEMP", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(0, 20, (uint8_t*)"ACC", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	data->acc = acc_measure();
	data->temp = temp->temperature;
	acc_display(display->acc, data->acc);
	temp_display(display->temp, data->temp);
	sseg_set('0', FALSE);
	rgb_set(0x00);
	vTaskResume(xRGBBlinkHandle);
	acc_interrupt_clear();
	acc_interrupt_start();
	state->modeState = MODE_FORWARD;
	state->accState = ACC_NORMAL;
	state->tempState = TEMP_NORMAL;
	state->lightState = LIGHT_OFF;
	ticks->x1sTicks = 0;
}

void to_mode_reverse(STATE* state, TICKS* ticks) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, (uint8_t*)"REVERSE", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	sseg_set(0xFF, TRUE);
//	rgb_set(0x00);
//	lights_start();
	state->modeState = MODE_REVERSE;
	state->accState = ACC_OFF;
	state->tempState = TEMP_OFF;
	state->lightState = LIGHT_NORMAL;
	ticks->x1sTicks = 0;
}

void in_mode_stationary(void) {

}

void in_mode_forward(TICKS* ticks, TEMP* temp, DATA* data, DISPLAY* display) {
	data->acc = acc_measure();
	data->temp = temp->temperature;
	ticks->x1sTicks++;
	sseg_set(SEGMENT_DISPLAY[ticks->x1sTicks % 16], FALSE);
	if (ticks->x1sTicks % 16 == 5 || ticks->x1sTicks % 16 == 10 || ticks->x1sTicks % 16 == 15) {
		temp_display(display->temp, data->temp);
		acc_display(display->acc, data->acc);
	}
	temp->temperature = 0;
	temp->halfPeriods = 0;
	temp->temperatureT1 = 0;
	temp->temperatureT2 = 0;
}

void in_mode_reverse(STATE* state, DATA* data, AMP* amp) {
//	data->light = lights_measure();
//	uint16_t led = lights_to_led(data->light);
//	if (data->light > LIGHT_THRESHOLD) {
//		if (state->lightState != LIGHT_HIGH) {
//			state->lightState = LIGHT_HIGH;
//			oled_putString(0, 40, (uint8_t*)"Obstacle near", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
//		}
//	} else {
//		state->lightState = LIGHT_NORMAL;
//		oled_putString(0, 40, (uint8_t*)"             ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
//		amp_stop();
//	}
//	pca9532_setLeds (led, ~led);
}
