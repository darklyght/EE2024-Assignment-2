/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/

#include "main.h"


STATE state = {MODE_STATIONARY, ACC_OFF, TEMP_OFF, LIGHT_OFF};
TICKS ticks = {0, 0};
TEMP temp = {0, 0, 0, 0};
uint8_t sw3Press = 0;
AMP amp = {0, -1};
DATA data = {0, 0, 0};
DISPLAY display = {"", "", ""};

void SysTick_Handler(void) {
	ticks.x1msTicks++;
	amp_volume_clock(&(amp.ampVolume));
}

void EINT0_IRQHandler(void) {
	sw3Press++; // Increment for each press within 1 second.
	sw3_timer_start(); // Start 1 second timer.
	LPC_SC->EXTINT |= (1<<0); // Clear interrupt.
}

void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO0IntStatR>>3) & 0x1) {
		state.accState = ACC_HIGH;
		oled_putString(0, 50, (uint8_t*)"Air bag released", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		LPC_GPIOINT->IO0IntEnR &= ~(1<<3);
		LPC_GPIOINT->IO0IntClr |= 1<<3;
	}
	if (((LPC_GPIOINT->IO0IntStatR>>2) & 0x1) || ((LPC_GPIOINT->IO0IntStatF>>2) & 0x1)) {
		temperature_measure(&state, &ticks, &temp);
		if (state.modeState == MODE_FORWARD && temp.temperature > TEMP_THRESHOLD) {
			state.tempState = TEMP_HIGH;
			oled_putString(0, 40, (uint8_t*)"Temp. too high", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
		}
		LPC_GPIOINT->IO0IntClr |= 1<<2;
	}
	if ((LPC_GPIOINT->IO2IntStatF>>5) & 0x1) {
		in_mode_reverse(&state, &data, &amp);
		light_clear_interrupt();
		LPC_GPIOINT->IO2IntClr |= 1<<5;
		LPC_GPIOINT->IO2IntEnF &= ~(1<<5);
	}
	if ((LPC_GPIOINT->IO0IntStatF>>24) & 0x1 || (LPC_GPIOINT->IO0IntStatF>>25) & 0x1) {
		amp_volume(rotary_read(), &(amp.ampVolume));
		LPC_GPIOINT->IO0IntClr |= 1<<24;
		LPC_GPIOINT->IO0IntClr |= 1<<25;
	}
}

void TIMER0_IRQHandler(void) {
	if (state.modeState != MODE_STATIONARY) {
		to_mode_stationary(&state, &ticks);
	} else if (sw3Press == 1) {
		to_mode_forward(&state, &ticks, &temp, &data, &display);
	} else {
		to_mode_reverse(&state, &ticks);
	}
	sw3Press = 0; // Reset number of switch 3 presses.
	LPC_TIM0->IR |= (1<<0); // Clear interrupt.
}

void TIMER1_IRQHandler(void) {
	if (state.modeState == MODE_FORWARD) {
		if (LPC_TIM1->IR>>2 & 0x1) {
			in_mode_forward(&ticks, &temp, &data, &display);
		}
		rgb_blink(&state);
	}
	if (state.modeState == MODE_REVERSE) {
		if (LPC_TIM1->IR>>3 & 0x1) {
			in_mode_reverse(&state, &data, &amp);
			timer_start_reverse(lights_to_beep(data.light));
		}
		LPC_GPIOINT->IO2IntEnF |= (1<<5);
		amp_beep(&state, &amp);
	}
	LPC_TIM1->IR |= 0xF;
}

void TIMER2_IRQHandler(void) {
	if (GPIO_ReadValue(0)>>26 & 0x1) {
		pwm_low();
	} else {
		pwm_high();
	}
	LPC_TIM2->IR |= 1<<0;
}

int main(void) {
	init_peripherals();
	to_mode_stationary(&state, &ticks);
	SysTick_Config(SystemCoreClock / 1000);
	while (1) {
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

