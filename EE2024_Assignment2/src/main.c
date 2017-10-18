/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/

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

volatile MODE_STATE modeState;
volatile ACCEL_STATE accelState;
volatile TEMP_STATE tempState;
volatile RGB_STATE rgbState;
volatile OBSTACLE_STATE obstacleState;
volatile MODE_STATE modeStateNext;

uint8_t MODE_DISPLAY[3][11] = {
		"STATIONARY",
		"FORWARD",
		"REVERSE"
};
uint8_t ACCX_DISPLAY[16] = "";
uint8_t TEMP_DISPLAY[16] = "";
uint8_t SEGMENT_DISPLAY[16] = "0123456789ABCDEF";

volatile uint32_t x1msTicks = 0;
volatile uint32_t x333msTicks = 0;
volatile uint32_t x1sTicks = 0;

int8_t accX = 0;
int8_t accY = 0;
int8_t accZ = 0;
int8_t accXCal = 0;
int8_t accYCal = 0;
int8_t accZCal = 0;

uint32_t sw3T = 0;

uint16_t luxVal = 0;
uint32_t speakerTicks = 0;
uint32_t pwmTicks = 0;
uint8_t pwmOn = 0;

int32_t temp = 0;
uint32_t tempT1 = 0;
uint32_t tempT2 = 0;
uint32_t tempTicks = 0;

static void init_peripherals(void);
static void init_gpio(void);
static void init_ssp(void);
static void init_i2c(void);
static void init_uart(void);
static void init_states(void);
static int abs(int num);

static void to_mode_stationary(void);
static void to_mode_forward(void);
static void to_mode_reverse(void);
static void in_mode_stationary(void);
static void in_mode_forward(void);
static void in_mode_reverse(void);

static void rgb_set(uint8_t ledMask);
static void rgb_blink(void);
static void acc_display_gen(void);
static void temp_display_gen(void);
static uint16_t obstacle_led_gen(uint16_t light);
static uint32_t obstacle_speaker_gen(uint16_t light);

static void init_peripherals(void) {
	init_gpio();
	init_ssp();
	init_i2c();
	init_uart();
	acc_init();
	light_init();
	rgb_init();
	led7seg_init();
	pca9532_init();
	oled_init();
	oled_clearScreen(OLED_COLOR_BLACK);
	NVIC_EnableIRQ(EINT3_IRQn);
}

static void init_gpio(void) {
	PINSEL_CFG_Type PinCfg;

	// Initialise temperature sensor with interrupt.
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PinCfg.Pinmode = 0;
	PinCfg.OpenDrain = 0;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<2), 0);
	LPC_GPIOINT->IO0IntEnR |= 1<<2;
	LPC_GPIOINT->IO0IntEnF |= 1<<2;

	// Initialise SW3 with interrupt
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<10), 0);
	LPC_GPIOINT->IO2IntEnF |= 1<<10;

	GPIO_SetDir(2, 1<<0, 1);
	GPIO_SetDir(2, 1<<1, 1);

	GPIO_SetDir(0, 1<<27, 1);
	GPIO_SetDir(0, 1<<28, 1);
	GPIO_SetDir(2, 1<<13, 1);
	GPIO_SetDir(0, 1<<26, 1);

	GPIO_ClearValue(0, 1<<27);
	GPIO_ClearValue(0, 1<<28);
	GPIO_ClearValue(2, 1<<13);
}

static void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);
}

static void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

static void init_uart(void) {
	UART_CFG_Type uartCfg;
	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;
	//pin select for uart3;
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	//supply power & setup working parameters for uart3
	UART_Init(LPC_UART3, &uartCfg);
	//enable transmit for uart3
	UART_TxCmd(LPC_UART3, ENABLE);
}

static void init_states(void) {
	modeState = MODE_STATIONARY;
	rgbState = BLINK_OFF;
	accelState = ACCEL_OFF;
	tempState = TEMP_OFF;
	obstacleState = OBSTACLE_OFF;
}

static void acc_display_gen(void) {
	char s[16] = "";
	strcpy((char*)ACCX_DISPLAY, "");
	strcat((char*)ACCX_DISPLAY, "X ACCEL: ");
	sprintf(s, "%.1f ", accX / ACC_DIV);
	strcat((char*)ACCX_DISPLAY, s);
}

static int abs(int num) {
	return num < 0 ? -num : num;
}

static void to_mode_stationary(void) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	led7seg_setChar(0xFF, TRUE);
	pca9532_setLeds(0x0000, 0xFFFF);
	rgb_set(0x00);
	rgbState = BLINK_OFF;
	acc_setMode(ACC_MODE_STANDBY);
	accelState = ACCEL_OFF;
	tempState = TEMP_OFF;
	obstacleState = OBSTACLE_OFF;
	light_shutdown();
	LPC_GPIOINT->IO0IntEnR &= !(1<<2);
	LPC_GPIOINT->IO0IntEnF &= !(1<<2);
}

static void to_mode_forward(void) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	rgbState = BLINK_OFF;
	acc_setMode(ACC_MODE_MEASURE);
	accelState = ACCEL_NORMAL;
	tempState = TEMP_NORMAL;
	obstacleState = OBSTACLE_OFF;
	temp_display_gen();
	acc_display_gen();
	oled_putString(0, 10, ACCX_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	oled_putString(0, 20, TEMP_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	LPC_GPIOINT->IO0IntEnR |= (1<<2);
	LPC_GPIOINT->IO0IntEnF |= (1<<2);
	x1msTicks = 0;
	x1sTicks = 0;
}

static void to_mode_reverse(void) {
	oled_clearScreen(OLED_COLOR_BLACK);
	oled_putString(0, 0, MODE_DISPLAY[modeStateNext], OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	rgbState = BLINK_OFF;
	accelState = ACCEL_NORMAL;
	tempState = TEMP_NORMAL;
	obstacleState = OBSTACLE_FAR;
	light_enable();
	light_setMode(LIGHT_MODE_D1);
	light_setWidth(LIGHT_WIDTH_12BITS);
	light_setRange(LIGHT_RANGE_4000);
	light_setLoThreshold(OBSTACLE_NEAR_THRESHOLD);
	light_setIrqInCycles(LIGHT_CYCLE_8);
	LPC_GPIOINT->IO0IntEnR &= !(1<<2);
	LPC_GPIOINT->IO0IntEnF &= !(1<<2);
	x1msTicks = 0;
	x1sTicks = 0;
}

static void in_mode_stationary(void) {
}

static void in_mode_forward(void) {
	if (x1msTicks - x333msTicks >= 333) {
		x333msTicks = x1msTicks;
		rgb_blink();
	}
	if (x1msTicks - x1sTicks >= 1000) {
		x1sTicks = x1msTicks;
		led7seg_setChar(SEGMENT_DISPLAY[((x1sTicks / 1000) - 1) % 16], FALSE);
		acc_read(&accX, &accY, &accZ);
		tempT1 = 0;
		tempT2 = 0;
		if (abs(accX) / ACC_DIV > ACC_THRESHOLD) {
			accelState = ACCEL_HIGH;
			if (tempState == TEMP_HIGH) {
				rgbState = BLINK_BOTH;
			} else {
				rgbState = BLINK_BLUE;
			}
			oled_putString(0, 40, (uint8_t*)"Air bag released", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
		if (abs(temp > TEMP_HIGH_WARNING)) {
			tempState = TEMP_HIGH;
			if (accelState == ACCEL_HIGH) {
				rgbState = BLINK_BOTH;
			} else {
				rgbState = BLINK_RED;
			}
			oled_putString(0, 50, (uint8_t*)"Temp. too high", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
		if ((((x1sTicks / 1000) - 1) % 16 == 5) || (((x1sTicks / 1000) - 1) % 16 == 10) || (((x1sTicks / 1000) - 1) % 16 == 15)) {
			temp_display_gen();
			acc_display_gen();
			oled_putString(0, 10, ACCX_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			oled_putString(0, 20, TEMP_DISPLAY, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
	}
}

static void in_mode_reverse(void) {
	if (pwmOn && x1msTicks - pwmTicks >= 1) {
		pwmTicks = x1msTicks;
		NOTE_PIN_HIGH();
	} else {
		NOTE_PIN_LOW();
	}
	if (obstacleState == OBSTACLE_NEAR && x1msTicks - speakerTicks >= obstacle_speaker_gen(luxVal)) {
		speakerTicks = x1msTicks;
		if (pwmOn) {
			pwmOn = 0;
		} else {
			pwmOn = 1;
		}
	}
	if (x1msTicks - x1sTicks >= 1000) {
		x1sTicks = x1msTicks;
		luxVal = light_read();
		pca9532_setLeds(obstacle_led_gen(luxVal), ~obstacle_led_gen(luxVal));
		if (luxVal > OBSTACLE_NEAR_THRESHOLD) {
			obstacleState = OBSTACLE_NEAR;
			oled_putString(0, 40, (uint8_t*)"Obstacle near", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		} else {
			obstacleState = OBSTACLE_FAR;
			oled_putString(0, 40, (uint8_t*)"             ", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
	}
}

static void rgb_set(uint8_t ledMask) {
	if ((ledMask & RGB_RED) != 0) {
		GPIO_SetValue(2, 1);
	} else {
		GPIO_ClearValue(2, 1);
	}
	if ((ledMask & RGB_BLUE) != 0) {
		GPIO_SetValue(0, (1<<26));
	} else {
		GPIO_ClearValue(0, (1<<26));
	}
}

static void rgb_blink(void) {
	switch (rgbState) {
	case BLINK_OFF:
		rgb_set(0x00);
		break;
	case BLINK_BLUE:
		if ((GPIO_ReadValue(2) & 0x1) || (GPIO_ReadValue(0)>>26) & 0x1) {
			rgb_set(0x00);
		} else {
			rgb_set(0x02);
		}
		break;
	case BLINK_RED:
		if ((GPIO_ReadValue(2) & 0x1) || (GPIO_ReadValue(0)>>26) & 0x1) {
			rgb_set(0x00);
		} else {
			rgb_set(0x01);
		}
		break;
	case BLINK_BOTH:
		if (GPIO_ReadValue(2) & 0x1) {
			rgb_set(0x02);
		} else {
			rgb_set(0x01);
		}
		break;
	}
}

static void temp_display_gen(void) {
	char s[16] = "";
	strcpy((char*)TEMP_DISPLAY, "");
	strcat((char*)TEMP_DISPLAY, "TEMP: ");
	sprintf(s, "%.1f", temp/10.0);
	strcat((char*)TEMP_DISPLAY, s);
}

static uint16_t obstacle_led_gen(uint16_t light) {
	// Uses a log scale since human light perception is logarithmic.
	if (light < 1) {
		return 0xFFFF;
	} else if (light < 3) {
		return 0x7FFF;
	} else if (light < 5) {
		return 0x3FFF;
	} else if (light < 8) {
		return 0x1FFF;
	} else if (light < 13) {
		return 0x0FFF;
	} else if (light < 22) {
		return 0x07FF;
	} else if (light < 38) {
		return 0x03FF;
	} else if (light < 63) {
		return 0x01FF;
	} else if (light < 106) {
		return 0x00FF;
	} else if (light < 178) {
		return 0x007F;
	} else if (light < 299) {
		return 0x003F;
	} else if (light < 501) {
		return 0x001F;
	} else if (light < 841) {
		return 0x000F;
	} else if (light < 1413) {
		return 0x0007;
	} else if (light < 2371) {
		return 0x0003;
	} else {
		return 0x0000;
	}
}

static uint32_t obstacle_speaker_gen(uint16_t light) {
	if (light < 100) {
		return 0;
	} else if (light < 501) {
		return 499;
	} else if (light < 1413) {
		return 249;
	} else {
		return 124;
	}
}

void SysTick_Handler(void) {
	x1msTicks++;
}

void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO2IntStatF>>10) & 0x1) {
		if (modeState == MODE_STATIONARY && !sw3T) {
			sw3T = x1msTicks;
			modeStateNext = MODE_FORWARD;
		} else if ((modeState == MODE_STATIONARY) && (x1msTicks - sw3T <= 1000)) {
			modeStateNext = MODE_REVERSE;
		} else if ((modeState == MODE_FORWARD) || (modeState == MODE_REVERSE)){
			sw3T = x1msTicks;
			modeStateNext = MODE_STATIONARY;
		}
		LPC_GPIOINT->IO2IntClr |= 1<<10;
	}
	if (((LPC_GPIOINT->IO0IntStatR>>2) & 0x1) || ((LPC_GPIOINT->IO0IntStatF>>2) & 0x1)) {
		if (!tempT1 && !tempT2) {
			tempT1 = x1msTicks;
		} else if (tempT1 && !tempT2) {
			tempTicks++;
			if (tempTicks == NUM_HALF_PERIODS) {
				tempT2 = x1msTicks;
				if (tempT2 > tempT1) {
					tempT2 = tempT2 - tempT1;
				}
				else {
					tempT2 = (0xFFFFFFFF - tempT1 + 1) + tempT2;
				}
				temp = (2*1000*tempT2) / (NUM_HALF_PERIODS*TEMP_SCALAR_DIV10) - 2731;
				tempTicks = 0;
			}
		}
		LPC_GPIOINT->IO0IntClr |= 1<<2;
	}
}

int main (void) {
	init_peripherals();
	init_states();
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}
	to_mode_stationary();
	while (1) {
		if (sw3T && (x1msTicks - sw3T > 1000)) {
			switch (modeStateNext) {
			case MODE_STATIONARY:
				to_mode_stationary();
				break;
			case MODE_FORWARD:
				to_mode_forward();
				break;
			case MODE_REVERSE:
				to_mode_reverse();
				break;
			}
			modeState = modeStateNext;
			sw3T = 0;
		}
		switch (modeState) {
		case MODE_STATIONARY:
			in_mode_stationary();
			break;
		case MODE_FORWARD:
			in_mode_forward();
			break;
		case MODE_REVERSE:
			in_mode_reverse();
			break;
		}
	}
	return 0;
}

void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
