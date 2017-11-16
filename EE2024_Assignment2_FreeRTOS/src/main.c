/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/

#include "main.h"
#include "cats_task_commons.h"

STATE state = {MODE_STATIONARY, ACC_OFF, TEMP_OFF, LIGHT_OFF};
TICKS ticks = {0};
TEMP temp = {0, 0, 0, 0};
//uint8_t sw3 = 0;
uint8_t log = 0;
AMP amp = {0, 0};
DATA data = {0, 0, 0};
DISPLAY display = {"", "", ""};

static void vSwitchModeTask(void *pvParameters) {
	while (1) {
		xSemaphoreTake(switchModeSemaphore, portMAX_DELAY);
		switch (state.modeState) {
		case MODE_STATIONARY:
			vTaskDelay(configTICK_RATE_HZ/1);
			if (xSemaphoreTake(switchModeSemaphore, 0) == pdFAIL) {
				to_mode_forward(&state, &ticks, &temp, &data, &display);
				state.modeState = MODE_FORWARD;
			} else {
				to_mode_reverse(&state, &ticks, &display);
				state.modeState = MODE_REVERSE;
			}
			break;
		case MODE_FORWARD:
			to_mode_stationary(&state, &ticks);
			state.modeState = MODE_STATIONARY;
			break;
		case MODE_REVERSE:
			to_mode_stationary(&state, &ticks);
			state.modeState = MODE_STATIONARY;
			break;
		}
	}
}

static void vModeTask(void *pvParameters) {
	portTickType xLastWakeTime = xTaskGetTickCount();;
	while (1) {
		switch (state.modeState) {
		case MODE_STATIONARY:
			in_mode_stationary();
			break;
		case MODE_FORWARD:
			in_mode_forward(&state, &ticks, &temp, &data, &display);
			break;
		case MODE_REVERSE:
			in_mode_reverse(&state, &data, &amp, &display);
			lights_to_led_change(data.light);
			NVIC_EnableIRQ(EINT2_IRQn);
			break;
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1);
	}
}

static void vLogger(void *pvParameters) {
	portTickType xLastWakeTime;
	char s[40] = "";
	while (1) {
		xSemaphoreTake(logSemaphore, portMAX_DELAY);
		switch(state.modeState) {
		case MODE_STATIONARY:
			set_uart_message(display.uart, "STATIONARY MODE\r\n");
			UART_Send(LPC_UART3, display.uart, strlen((char*)display.uart), BLOCKING);
			break;
		case MODE_FORWARD:
			data.acc = acc_measure();
			data.temp = temp.temperature;
			sprintf(s, "FORWARD MODE | TEMPERATURE: %.2f ACCELERATION: %.2f\r\n", data.temp / 10.0, data.acc);
			set_uart_message(display.uart, s);
			UART_Send(LPC_UART3, display.uart, strlen((char*)display.uart), BLOCKING);
			break;
		case MODE_REVERSE:
			data.light = lights_measure();
			sprintf(s, "REVERSE MODE | LIGHT INTENSITY: %d\r\n", (int)data.light);
			set_uart_message(display.uart, s);
			UART_Send(LPC_UART3, display.uart, strlen((char*)display.uart), BLOCKING);
			break;
		}
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/5);
	}
}

static void vRGBBlink(void *pvParameters) {
	portTickType xLastWakeTime = xTaskGetTickCount();;
	while(1) {
		rgb_blink(&state);
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/3);
	}
}

static void vAmpBeep(void *pvParameters) {
	portTickType xLastWakeTime = xTaskGetTickCount();
	float delay;
	while (1) {
		delay = lights_to_beep(data.light);
		amp_beep(&amp);
		vTaskDelayUntil(&xLastWakeTime, (int)configTICK_RATE_HZ/delay);
	}
}

static void vAmpVolume(void *pvParameters) {
	portTickType xLastWakeTime;
	while (1) {
		xSemaphoreTake(ampVolumeSemaphore, portMAX_DELAY);
		GPIO_ClearValue(2, 1<<6);
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/250);
		GPIO_SetValue(2, 1<<6);
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/250);
		GPIO_ClearValue(2, 1<<6);
	}
}

void EINT0_IRQHandler(void) {
	giveAndYield(switchModeSemaphore);
	LPC_SC->EXTINT |= (1<<0); // Clear interrupt.
}

void EINT1_IRQHandler(void) {
	temperature_measure(&temp);
	if (state.tempState == TEMP_NORMAL && temp.temperature > TEMP_THRESHOLD) {
		oled_putString(0, 40, (uint8_t*)"Temp. too high", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		state.tempState = TEMP_HIGH;
	}
	LPC_SC->EXTINT |= (1<<1); // Clear interrupt.
}

void EINT2_IRQHandler(void) {
	in_mode_reverse(&state, &data, &amp, &display);
	NVIC_DisableIRQ(EINT2_IRQn);
	light_clear_interrupt();
	LPC_SC->EXTINT |= (1<<2); // Clear interrupt.
}

void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO0IntStatR>>3) & 0x1) {
		oled_putString(0, 50, (uint8_t*)"Air bag released", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		state.accState = ACC_HIGH;
		LPC_GPIOINT->IO0IntEnR &= ~(1<<3);
		LPC_GPIOINT->IO0IntClr |= 1<<3;
	}
	if ((LPC_GPIOINT->IO0IntStatF>>24) & 0x1 || (LPC_GPIOINT->IO0IntStatF>>25) & 0x1) {
		amp_volume(rotary_read(), &(amp.ampVolume));
		LPC_GPIOINT->IO0IntClr |= 1<<24;
		LPC_GPIOINT->IO0IntClr |= 1<<25;
	}
}

void TIMER2_IRQHandler(void) {
	if (GPIO_ReadValue(0)>>26 & 0x1) {
		pwm_low();
	} else {
		pwm_high();
	}
	LPC_TIM2->IR |= 1<<0;
}

void UART3_IRQHandler(void) {
	uint8_t letter;
	UART_Receive(LPC_UART3, &letter, 1, BLOCKING);
	if (letter == 'o') {
		giveAndYield(switchModeSemaphore);
	} else if (letter == 'l') {
		giveAndYield(logSemaphore);
	}
}

int main(void) {
	init_peripherals();

	vSemaphoreCreateBinary(switchModeSemaphore);
	xSemaphoreTake(switchModeSemaphore, 0);
	vSemaphoreCreateBinary(ampVolumeSemaphore);
	vSemaphoreCreateBinary(logSemaphore);
	xSemaphoreTake(logSemaphore, 0);

	xTaskCreate(vSwitchModeTask, (signed char *) "vSwitchModeTask",
						configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 4UL),
						NULL);

	xTaskCreate(vModeTask, (signed char *) "vModeTask",
						configMINIMAL_STACK_SIZE*20, NULL, (tskIDLE_PRIORITY + 3UL),
						&xModeTaskHandle);

	xTaskCreate(vLogger, (signed char *) "vLogger",
						configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 3UL),
						NULL);

	xTaskCreate(vRGBBlink, (signed char *) "vRGBBlink",
							configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
							&xRGBBlinkHandle);

	xTaskCreate(vAmpBeep, (signed char *) "vAmpBeep",
							configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
							&xAmpBeepHandle);

	xTaskCreate(vAmpVolume, (signed char *) "vAmpVolume",
							configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
							&xAmpVolumeHandle);

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

