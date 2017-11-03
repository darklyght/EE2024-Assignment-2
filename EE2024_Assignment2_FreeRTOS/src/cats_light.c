/*
 * cats_light.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_light.h"

void lights_init(void) {
	light_init();
	light_setWidth(LIGHT_WIDTH_12BITS);
	light_setRange(LIGHT_RANGE_4000);
	light_setIrqInCycles(LIGHT_CYCLE_8);
	light_setHiThreshold((uint32_t)LIGHT_THRESHOLD);
	light_setLoThreshold((uint32_t)0);
	light_clear_interrupt();

	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<5), 0);
	LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
}

uint32_t lights_measure(void) {
	return light_read();
}

uint16_t lights_to_led(uint32_t light) {
	if (light < 242) {
		return 0xFFFF;
	} else if (light < 484) {
		return 0x7FFF;
	} else if (light < 726) {
		return 0x3FFF;
	} else if (light < 968) {
		return 0x1FFF;
	} else if (light < 1210) {
		return 0x0FFF;
	} else if (light < 1452) {
		return 0x07FF;
	} else if (light < 1694) {
		return 0x03FF;
	} else if (light < 1936) {
		return 0x01FF;
	} else if (light < 2178) {
		return 0x00FF;
	} else if (light < 2420) {
		return 0x007F;
	} else if (light < 2662) {
		return 0x003F;
	} else if (light < 2904) {
		return 0x001F;
	} else if (light < 3146) {
		return 0x000F;
	} else if (light < 3388) {
		return 0x0007;
	} else if (light < 3630) {
		return 0x0003;
	} else {
		return 0x0000;
	}
}

void lights_to_led_change(uint32_t light) {
	portTickType xLastWakeTime;
	uint16_t currOn = pca9532_getLedState(TRUE);
	uint16_t toTurnOn = lights_to_led(light);
	int i;
	if ((int)(currOn - toTurnOn) < 0) {
		for (i = 0; i < 16; i++) {
			if (~currOn & toTurnOn & (1<<i)) {
				pca9532_setLeds (currOn | 1<<i, ~(currOn | 1<<i));
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
				currOn = pca9532_getLedState(TRUE);
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
			}
		}
	} else if ((int)(currOn - toTurnOn) > 0) {
		for (i = 15; i >= 0; i--) {
			if (~toTurnOn & currOn & (1<<i)) {
				pca9532_setLeds (currOn & ~(1<<i), ~(currOn & ~(1<<i)));
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
				currOn = pca9532_getLedState(TRUE);
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/100);
			}
		}
	}
}

float lights_to_beep(uint32_t light) {
	if (light < 242) {
		return 3;
	} else if (light < 484) {
		return 3.5;
	} else if (light < 726) {
		return 4;
	} else if (light < 968) {
		return 4.5;
	} else if (light < 1210) {
		return 5;
	} else if (light < 1452) {
		return 5.5;
	} else if (light < 1694) {
		return 6;
	} else if (light < 1936) {
		return 6.5;
	} else if (light < 2178) {
		return 7;
	} else if (light < 2420) {
		return 7.5;
	} else if (light < 2662) {
		return 8;
	} else if (light < 2904) {
		return 8.5;
	} else if (light < 3146) {
		return 9;
	} else if (light < 3388) {
		return 9.5;
	} else if (light < 3630) {
		return 10;
	} else {
		return 10.5;
	}
}

void lights_start(void) {
	LPC_GPIOINT->IO2IntEnF |= (1<<5);
	light_enable();
	light_setWidth(LIGHT_WIDTH_12BITS);
	light_setRange(LIGHT_RANGE_4000);
}

void lights_stop(void) {
	light_shutdown();
	LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
}

uint8_t lights_read_interrupt(void) {
	return light_getIrqStatus();
}

void light_clear_interrupt(void) {
	light_clearIrqStatus();
}
