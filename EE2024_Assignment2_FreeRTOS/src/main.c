/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/

#include "main.h"

volatile STATE state = {MODE_STATIONARY, ACC_OFF, TEMP_OFF, LIGHT_OFF};
volatile TICKS ticks = {0, 0};
volatile TEMP temp = {0, 0, 0, 0};
volatile uint8_t sw3 = 0;
volatile AMP amp = {0, -1};
volatile DATA data = {0, 0, 0};
volatile DISPLAY display = {"", "", ""};

	portTickType xLastWakeTime;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1000);
		ticks.x1msTicks++;
	}
}

static void vSwitchModeTask(void *pvParameters) {
	portTickType xLastWakeTime;
	while (1) {
		if (sw3 > 0) {
			switch (state.modeState) {
			case MODE_STATIONARY:
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1);
				if (sw3 == 1) {
					to_mode_forward(&state, &ticks, &temp, &data, &display);
					sw3 = 0;
					state.modeState = MODE_FORWARD;
				} else {
					to_mode_reverse(&state, &ticks);
					sw3 = 0;
					state.modeState = MODE_REVERSE;
				}
				break;
			case MODE_FORWARD:
				to_mode_stationary(&state, &ticks);
				sw3 = 0;
				state.modeState = MODE_STATIONARY;
				break;
			case MODE_REVERSE:
				to_mode_stationary(&state, &ticks);
				sw3 = 0;
				state.modeState = MODE_STATIONARY;
				break;
			}
		}
	}
}

static void vModeTask(void *pvParameters) {
	portTickType xLastWakeTime;
	while (1) {
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1);
		switch (state.modeState) {
		case MODE_STATIONARY:
			in_mode_stationary();
			break;
		case MODE_FORWARD:
			in_mode_forward(&ticks, &temp, &data, &display);
			break;
		case MODE_REVERSE:
			in_mode_reverse(&state, &data, &amp);
			break;
		}
	}
}

static void vRGBBlink (void *pvParameters) {
	portTickType xLastWakeTime;
	while(1) {
		rgb_blink(&state);
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/3);
	}
}

void EINT0_IRQHandler(void) {
	sw3++; // Increment for each press within 1 second.
	LPC_SC->EXTINT |= (1<<0); // Clear interrupt.
}

void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO0IntStatR>>3) & 0x1) {
		oled_putString(0, 50, (uint8_t*)"Air bag released", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		state.accState = ACC_HIGH;
		LPC_GPIOINT->IO0IntEnR &= ~(1<<3);
		LPC_GPIOINT->IO0IntClr |= 1<<3;
	}
	if (((LPC_GPIOINT->IO0IntStatR>>2) & 0x1) || ((LPC_GPIOINT->IO0IntStatF>>2) & 0x1)) {
		temperature_measure(&ticks, &temp);
		if (state.tempState == TEMP_NORMAL && temp.temperature > TEMP_THRESHOLD) {
			oled_putString(0, 40, (uint8_t*)"Temp. too high", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
			state.tempState = TEMP_HIGH;
		}
		LPC_GPIOINT->IO0IntClr |= 1<<2;
	}
//	if ((LPC_GPIOINT->IO2IntStatF>>5) & 0x1) {
//		in_mode_reverse(&state, &data, &amp);
//		light_clear_interrupt();
//		LPC_GPIOINT->IO2IntClr |= 1<<5;
//		LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
//	}
//	if ((LPC_GPIOINT->IO0IntStatF>>24) & 0x1 || (LPC_GPIOINT->IO0IntStatF>>25) & 0x1) {
//		amp_volume(rotary_read(), &(amp.ampVolume));
//		LPC_GPIOINT->IO0IntClr |= 1<<24;
//		LPC_GPIOINT->IO0IntClr |= 1<<25;
//	}
}
//
//
//void TIMER2_IRQHandler(void) {
//	if (GPIO_ReadValue(0)>>26 & 0x1) {
//		pwm_low();
//	} else {
//		pwm_high();
//	}
//	LPC_TIM2->IR |= 1<<0;
//}

int main(void) {
	init_peripherals();

						configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
						(xTaskHandle *) NULL);

	xTaskCreate(vSwitchModeTask, (signed char *) "vSwitchModeTask",
					configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL),
					(xTaskHandle *) NULL);

	xTaskCreate(vModeTask, (signed char *) "vModeTask",
						configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL),
						(xTaskHandle *) NULL);

	xTaskCreate(vRGBBlink, (signed char *) "vRGBBlink",
							configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL),
							(xTaskHandle *) &xRGBBlinkHandle);

	to_mode_stationary(&state, &ticks);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}

