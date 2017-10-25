/*
 * header.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef HEADER_H_
#define HEADER_H_

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"

#include "joystick.h"
#include "rotary.h"
#include "pca9532.h"
#include "acc.h"
#include "oled.h"
#include "temp.h"
#include "rgb.h"
#include "led7seg.h"
#include "light.h"

#include <stdio.h>
#include <string.h>

#define TEMP_DIV 1
#define TOTAL_HALF_PERIODS 340
#define TEMP_THRESHOLD 280
#define ACC_THRESHOLD 0.4
#define ACC_DIV_MEASURE 64.0
#define ACC_DIV_LEVEL 16.0
#define LIGHT_THRESHOLD 3000
#define LIGHT_BEEP_THRESHOLD 100

typedef enum {
	MODE_STATIONARY,
	MODE_FORWARD,
	MODE_REVERSE
} MODE_STATE;

typedef enum {
	ACC_OFF,
	ACC_NORMAL,
	ACC_HIGH
} ACC_STATE;

typedef enum {
	TEMP_OFF,
	TEMP_NORMAL,
	TEMP_HIGH
} TEMP_STATE;

typedef enum {
	LIGHT_OFF,
	LIGHT_NORMAL,
	LIGHT_HIGH
} LIGHT_STATE;

typedef struct {
	uint32_t x1msTicks;
	uint32_t x1sTicks;
} TICKS;

typedef struct {
	MODE_STATE modeState;
	ACC_STATE accState;
	TEMP_STATE tempState;
	LIGHT_STATE lightState;
} STATE;

typedef struct {
	int32_t temperature;
	uint32_t halfPeriods;
	uint32_t temperatureT1;
	uint32_t temperatureT2;
} TEMP;

typedef struct {
	uint8_t pwmOn;
	uint8_t ampVolume;
} AMP;

typedef struct {
	float acc;
	int32_t temp;
	uint32_t light;
} DATA;

typedef struct {
	uint8_t acc[6];
	uint8_t temp[6];
	uint8_t light[6];
} DISPLAY;

#endif /* HEADER_H_ */
