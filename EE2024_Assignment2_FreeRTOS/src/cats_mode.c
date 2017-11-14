/*
 * cats_mode.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_mode.h"
#include "cats_task_commons.h"

uint8_t SEGMENT_DISPLAY[16] = "0123456789ABCDEF";
uint32_t counter = 0;

void to_mode_stationary(STATE* state, TICKS* ticks) {
	amp_stop();
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, (uint8_t*)"STATIONARY", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	rgb_set(0x00);
	sseg_set(0xFF, TRUE);
	vTaskSuspend(xRGBBlinkHandle);
	vTaskSuspend(xAmpBeepHandle);
	vTaskSuspend(xAmpVolumeHandle);
	acc_interrupt_stop();
	acc_interrupt_clear();
	lights_stop();
	pca9532_setLeds(0x0000, 0xFFFF);
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
	set_uart_message(display->uart, "Entering Forward mode.\r\n");
	UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
	state->modeState = MODE_FORWARD;
	state->accState = ACC_NORMAL;
	state->tempState = TEMP_NORMAL;
	state->lightState = LIGHT_OFF;
	ticks->x1sTicks = 0;
}

void to_mode_reverse(STATE* state, TICKS* ticks, DISPLAY* display) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, (uint8_t*)"REVERSE", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	rgb_set(0x00);
	lights_start();
	vTaskResume(xAmpBeepHandle);
	vTaskResume(xAmpVolumeHandle);
	set_uart_message(display->uart, "Entering Reverse mode.\r\n");
	UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
	state->modeState = MODE_REVERSE;
	state->accState = ACC_OFF;
	state->tempState = TEMP_OFF;
	state->lightState = LIGHT_NORMAL;
	ticks->x1sTicks = 0;
}

void in_mode_stationary(void) {

}

void in_mode_forward(STATE* state, TICKS* ticks, TEMP* temp, DATA* data, DISPLAY* display) {
	sseg_set(SEGMENT_DISPLAY[ticks->x1sTicks % 16], FALSE);
	data->acc = acc_measure();
	data->temp = temp->temperature;
	if (ticks->x1sTicks % 16 == 5 || ticks->x1sTicks % 16 == 10 || ticks->x1sTicks % 16 == 15) {
		temp_display(display->temp, data->temp);
		acc_display(display->acc, data->acc);
		if (ticks->x1sTicks % 16 == 15) {
			if (state->tempState == TEMP_HIGH) {
				set_uart_message(display->uart, "Temperature too high.\r\n");
				UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
			}
			if (state->accState == ACC_HIGH) {
				set_uart_message(display->uart, "Collision has been detected.\r\n");
				UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
			}
			uart_display_gen(display->uart, counter, data->acc, data->temp);
			UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
			counter++;
		}
	}
	temp->halfPeriods = 0;
	temp->temperatureT1 = 0;
	temp->temperatureT2 = 0;
	ticks->x1sTicks++;
}

void in_mode_reverse(STATE* state, DATA* data, AMP* amp, DISPLAY* display) {
	data->light = lights_measure();
	if (data->light > LIGHT_THRESHOLD) {
		if (state->lightState != LIGHT_HIGH) {
			oled_putString(0, 40, (uint8_t*)"Obstacle near", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			state->lightState = LIGHT_HIGH;
			set_uart_message(display->uart, "Obstacle too near.\r\n");
			UART_Send(LPC_UART3, display->uart, strlen((char*)display->uart), BLOCKING);
		}
	} else {
		oled_putString(0, 40, (uint8_t*)"             ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		state->lightState = LIGHT_NORMAL;
	}
}
