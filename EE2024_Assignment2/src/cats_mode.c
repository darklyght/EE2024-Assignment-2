/*
 * cats_mode.c
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#include "cats_mode.h"

uint8_t MODE_DISPLAY[3][11] = {
		"STATIONARY",
		"FORWARD",
		"REVERSE"
};
uint8_t SEGMENT_DISPLAY[16] = "0123456789ABCDEF";
uint32_t messageCounter = 0;
uint8_t obstacleMessageSent = 0;

void to_mode_stationary(STATE* catsState) {
	acc_interrupt_clear();
	acc_interrupt_init();
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[catsState->modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	led7seg_setChar(0xFF, TRUE);
	pca9532_setLeds(0x0000, 0xFFFF);
	rgb_set(0x00);
	catsState->modeState = MODE_STATIONARY;
	catsState->accelState = ACCEL_OFF;
	catsState->tempState = TEMP_OFF;
	catsState->rgbState = BLINK_OFF;
	catsState->obstacleState = OBSTACLE_OFF;
//	acc_setMode(ACC_MODE_STANDBY);
	light_shutdown();
	LPC_TIM0->TCR &= ~(1 << 0);
	LPC_GPIOINT->IO0IntEnR &= ~(1<<2);
	LPC_GPIOINT->IO0IntEnF &= ~(1<<2);
}

void to_mode_forward(STATE* catsState, TICKS* catsTicks, ACC* catsAcc, TEMP_VARS* catsTemp, uint8_t* ACCX_DISPLAY, uint8_t* TEMP_DISPLAY, uint8_t* UART_DISPLAY) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[catsState->modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	catsState->modeState = MODE_FORWARD;
	catsState->accelState = ACCEL_NORMAL;
	catsState->tempState = TEMP_NORMAL;
	catsState->rgbState = BLINK_OFF;
	catsState->obstacleState = OBSTACLE_OFF;
	acc_setMode(ACC_MODE_MEASURE);
	pca9532_setLeds(0x0000, 0xFFFF);
	set_uart_message(UART_DISPLAY, "Entering Forward mode.\r\n");
	UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
	acc_display_gen(ACCX_DISPLAY, catsAcc->accX);
	temp_display_gen(TEMP_DISPLAY, catsTemp->temp);
	oled_putString(0, 10, ACCX_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(0, 20, TEMP_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	LPC_GPIOINT->IO0IntEnR |= (1<<2);
	LPC_GPIOINT->IO0IntEnF |= (1<<2);
	catsTicks->x1msTicks = 0;
	catsTicks->x333msTicks = 0;
	catsTicks->x1sTicks = 0;
}

void to_mode_reverse(STATE* catsState, TICKS* catsTicks, uint8_t* UART_DISPLAY) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[catsState->modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	catsState->modeState = MODE_REVERSE;
	catsState->accelState = ACCEL_NORMAL;
	catsState->tempState = TEMP_NORMAL;
	catsState->rgbState = BLINK_OFF;
	catsState->obstacleState = OBSTACLE_FAR;
	set_uart_message(UART_DISPLAY, "Entering Reverse mode.\r\n");
	UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
	light_enable();
	light_setMode(LIGHT_MODE_D1);
	light_setWidth(LIGHT_WIDTH_12BITS);
	light_setRange(LIGHT_RANGE_4000);
	light_setLoThreshold(OBSTACLE_NEAR_THRESHOLD);
	light_setIrqInCycles(LIGHT_CYCLE_8);
	LPC_GPIOINT->IO0IntEnR &= ~(1<<2);
	LPC_GPIOINT->IO0IntEnF &= ~(1<<2);
	catsTicks->x1msTicks = 0;
	catsTicks->x1sTicks = 0;
}

void in_mode_stationary(STATE* catsState) {
}

void in_mode_forward(STATE* catsState, TICKS* catsTicks, ACC* catsAcc, TEMP_VARS* catsTemp, uint8_t* ACCX_DISPLAY, uint8_t* TEMP_DISPLAY, uint8_t* UART_DISPLAY) {
	if (catsTicks->x1msTicks - catsTicks->x333msTicks >= 333) {
		catsTicks->x333msTicks = catsTicks->x1msTicks;
		rgb_blink(catsState);
	}
	if (catsTicks->x1msTicks - catsTicks->x1sTicks >= 1000) {
		catsTicks->x1sTicks = catsTicks->x1msTicks;
		led7seg_setChar(SEGMENT_DISPLAY[((catsTicks->x1sTicks / 1000) - 1) % 16], FALSE);
		acc_read(&(catsAcc->accX), &(catsAcc->accY), &(catsAcc->accZ));
		catsTemp->tempT1 = 0;
		catsTemp->tempT2 = 0;
		if (abs(catsAcc->accX) / ACC_DIV > ACC_THRESHOLD) {
			catsState->accelState = ACCEL_HIGH;
			if (catsState->tempState == TEMP_HIGH) {
				catsState->rgbState = BLINK_BOTH;
			} else {
				catsState->rgbState = BLINK_BLUE;
			}
			oled_putString(0, 40, (uint8_t*)"Air bag released", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
		if (abs(catsTemp->temp > TEMP_HIGH_WARNING)) {
			catsState->tempState = TEMP_HIGH;
			if (catsState->accelState == ACCEL_HIGH) {
				catsState->rgbState = BLINK_BOTH;
			} else {
				catsState->rgbState = BLINK_RED;
			}
			oled_putString(0, 50, (uint8_t*)"Temp. too high", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
		if ((((catsTicks->x1sTicks / 1000) - 1) % 16 == 5) || (((catsTicks->x1sTicks / 1000) - 1) % 16 == 10) || (((catsTicks->x1sTicks / 1000) - 1) % 16 == 15)) {
			acc_display_gen(ACCX_DISPLAY, catsAcc->accX);
			temp_display_gen(TEMP_DISPLAY, catsTemp->temp);
			oled_putString(0, 10, ACCX_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			oled_putString(0, 20, TEMP_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			if (((catsTicks->x1sTicks / 1000) - 1) % 16 == 15) {
				if (catsState->tempState == TEMP_HIGH) {
					set_uart_message(UART_DISPLAY, "Temperature too high.\r\n");
					UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
				}
				if (catsState->accelState == ACCEL_HIGH) {
					set_uart_message(UART_DISPLAY, "Collision has been detected.\r\n");
					UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
				}
				uart_display_gen(UART_DISPLAY, messageCounter, catsAcc->accX, catsTemp->temp);
				UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
				messageCounter++;
			}
		}
	}
}

void in_mode_reverse(STATE* catsState, TICKS* catsTicks, uint16_t* luxVal, SPEAKER* catsSpeaker, uint8_t* UART_DISPLAY) {
	if (catsState->obstacleState == OBSTACLE_NEAR && catsTicks->x1msTicks - catsSpeaker->speakerTicks >= obstacle_speaker_gen(*luxVal)) {
		catsSpeaker->speakerTicks = catsTicks->x1msTicks;
		if (catsSpeaker->pwmOn) {
			NOTE_PIN_LOW();
			LPC_TIM0->TCR &= ~(1 << 0);
			catsSpeaker->pwmOn = 0;
		} else {
			LPC_TIM0->TCR |= 1 << 0;
			catsSpeaker->pwmOn = 1;
		}
	}
	if (catsTicks->x1msTicks - catsTicks->x1sTicks >= 1000) {
		catsTicks->x1sTicks = catsTicks->x1msTicks;
		*luxVal = light_read();
		pca9532_setLeds(obstacle_led_gen(*luxVal), ~obstacle_led_gen(*luxVal));
		if (*luxVal > OBSTACLE_NEAR_THRESHOLD) {
			catsState->obstacleState = OBSTACLE_NEAR;
			oled_putString(0, 40, (uint8_t*)"Obstacle near", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			if (!obstacleMessageSent) {
				set_uart_message(UART_DISPLAY, "Obstacle too near.\r\n");
				UART_Send(LPC_UART3, UART_DISPLAY, strlen(UART_DISPLAY), BLOCKING);
				obstacleMessageSent = 1;
			}
		} else {
			catsState->obstacleState = OBSTACLE_FAR;
			LPC_TIM0->TCR &= ~(1 << 0);
			oled_putString(0, 40, (uint8_t*)"             ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			obstacleMessageSent = 0;
		}
	}
}
