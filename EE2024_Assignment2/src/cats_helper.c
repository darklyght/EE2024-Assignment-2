/*
 * cats_helper.c
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#include "cats_helper.h"

int abs(int num) {
	return num < 0 ? -num : num;
}

void rgb_set(uint8_t ledMask) {
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

void rgb_blink(STATE* catsState) {
	switch (catsState->rgbState) {
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

void acc_display_gen(uint8_t* ACCX_DISPLAY, int8_t accX) {
	char s[16] = "";
	strcpy((char*)ACCX_DISPLAY, "");
	strcat((char*)ACCX_DISPLAY, "X ACCEL: ");
	sprintf(s, "%.1f ", accX / ACC_DIV);
	strcat((char*)ACCX_DISPLAY, s);
}

void temp_display_gen(uint8_t* TEMP_DISPLAY, int32_t temp) {
	char s[16] = "";
	strcpy((char*)TEMP_DISPLAY, "");
	strcat((char*)TEMP_DISPLAY, "TEMP: ");
	sprintf(s, "%.1f", temp/10.0);
	strcat((char*)TEMP_DISPLAY, s);
}

uint16_t obstacle_led_gen(uint16_t light) {
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

uint32_t obstacle_speaker_gen(uint16_t light) {
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
