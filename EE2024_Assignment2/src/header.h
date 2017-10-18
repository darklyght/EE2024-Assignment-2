/*
 * header.h
 *
 *  Created on: Oct 18, 2017
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
#include "pca9532.h"
#include "acc.h"
#include "oled.h"
#include "temp.h"
#include "rgb.h"
#include "led7seg.h"
#include "light.h"

#include <stdio.h>
#include <string.h>

#define NOTE_PIN_HIGH() GPIO_SetValue(0, 1<<26)
#define NOTE_PIN_LOW()  GPIO_ClearValue(0, 1<<26)
#define ACC_THRESHOLD 0.2
#define ACC_DIV 63.0
#define TEMP_HIGH_WARNING 280
#define OBSTACLE_NEAR_THRESHOLD 100
#define TEMP_SCALAR_DIV10 1
#define NUM_HALF_PERIODS 340

typedef enum {
	MODE_STATIONARY,
	MODE_FORWARD,
	MODE_REVERSE
} MODE_STATE;

typedef enum {
	ACCEL_OFF,
	ACCEL_NORMAL,
	ACCEL_HIGH
} ACCEL_STATE;

typedef enum {
	TEMP_OFF,
	TEMP_NORMAL,
	TEMP_HIGH
} TEMP_STATE;

typedef enum {
	BLINK_OFF,
	BLINK_BLUE,
	BLINK_RED,
	BLINK_BOTH
} RGB_STATE;

typedef enum {
	OBSTACLE_OFF,
	OBSTACLE_FAR,
	OBSTACLE_NEAR
} OBSTACLE_STATE;

typedef struct {
	MODE_STATE modeState;
	ACCEL_STATE accelState;
	TEMP_STATE tempState;
	RGB_STATE rgbState;
	OBSTACLE_STATE obstacleState;
	MODE_STATE modeStateNext;
} STATE;

typedef struct {
	uint32_t x1msTicks;
	uint32_t x333msTicks;
	uint32_t x1sTicks;
} TICKS;

typedef struct {
	int8_t accX;
	int8_t accY;
	int8_t accZ;
} ACC;

typedef struct {
	int32_t temp;
	uint32_t tempT1;
	uint32_t tempT2;
	uint32_t tempTicks;
} TEMP_VARS;

typedef struct {
	uint32_t speakerTicks;
	uint32_t pwmTicks;
	uint8_t pwmOn;
} SPEAKER;

int abs(int num);

uint8_t MODE_DISPLAY[3][11];
uint8_t SEGMENT_DISPLAY[16];

#endif /* HEADER_H_ */
