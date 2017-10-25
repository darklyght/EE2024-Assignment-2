/*****************************************************************************
 *   A demo example using several of the peripherals on the base board
 *
 *   Copyright(C) 2011, EE2024
 *   All rights reserved.
 *
 ******************************************************************************/
#include "main.h"

uint8_t ACCX_DISPLAY[16] = "";
uint8_t TEMP_DISPLAY[16] = "";
uint8_t UART_DISPLAY[64] = "";

uint32_t sw3Ticks = 0;
uint16_t luxVal = 0;
uint8_t volumeAdjust = -1;

STATE catsState;
ACC catsAcc = {0, 0, 0};
TICKS catsTicks = {0, 0, 0};
TEMP_VARS catsTemp = {0, 0, 0, 0};
SPEAKER catsSpeaker = {0, 0, 0};

void SysTick_Handler(void) {
	catsTicks.x1msTicks++;
	acc_interrupt_clear();
	if (volumeAdjust >= 0 && volumeAdjust < 3) {
		GPIO_ClearValue(2, 1<<6);
		volumeAdjust++;
	} else if (volumeAdjust >= 3 && volumeAdjust < 6) {
		GPIO_SetValue(2, 1<<6);
		volumeAdjust++;
	} else if (volumeAdjust >= 6 && volumeAdjust < 9) {
		GPIO_ClearValue(2, 1<<6);
		volumeAdjust++;
	}
}

void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO2IntStatF>>10) & 0x1) {
		if (catsState.modeState == MODE_STATIONARY && !sw3Ticks) {
			sw3Ticks = catsTicks.x1msTicks;
			catsState.modeStateNext = MODE_FORWARD;
		} else if ((catsState.modeState == MODE_STATIONARY) && (catsTicks.x1msTicks - sw3Ticks <= 1000)) {
			catsState.modeStateNext = MODE_REVERSE;
		} else if ((catsState.modeState == MODE_FORWARD) || (catsState.modeState == MODE_REVERSE)){
			sw3Ticks = catsTicks.x1msTicks;
			catsState.modeStateNext = MODE_STATIONARY;
		}
		LPC_GPIOINT->IO2IntClr |= 1<<10;
	}
	if ((LPC_GPIOINT->IO0IntStatF>>24) & 0x1 || (LPC_GPIOINT->IO0IntStatF>>25) & 0x1) {
		volume_adjust(rotary_read(), &volumeAdjust);
		LPC_GPIOINT->IO0IntClr |= 1<<24;
		LPC_GPIOINT->IO0IntClr |= 1<<25;
	}
	if ((LPC_GPIOINT->IO0IntStatR>>3) & 0x1) {
//		printf("%d %d %d\n", (int)acc_interrupt_read(0x06), (int)acc_interrupt_read(0x07), (int)acc_interrupt_read(0x08));
		printf("%d\n", (int)acc_interrupt_read(0x0A));
		LPC_GPIOINT->IO0IntClr |= 1<<3;
//		printf("%d %d %d\n", (int)acc_interrupt_read(0x06), (int)acc_interrupt_read(0x07), (int)acc_interrupt_read(0x08));
//		printf("%d\n", (int)acc_interrupt_read(0x0A));
	}
	if (((LPC_GPIOINT->IO0IntStatR>>2) & 0x1) || ((LPC_GPIOINT->IO0IntStatF>>2) & 0x1)) {
		if (!catsTemp.tempT1 && !catsTemp.tempT2) {
			catsTemp.tempT1 = catsTicks.x1msTicks;
		} else if (catsTemp.tempT1 && !catsTemp.tempT2) {
			catsTemp.tempTicks++;
			if (catsTemp.tempTicks == NUM_HALF_PERIODS) {
				catsTemp.tempT2 = catsTicks.x1msTicks;
				if (catsTemp.tempT2 > catsTemp.tempT1) {
					catsTemp.tempT2 = catsTemp.tempT2 - catsTemp.tempT1;
				}
				else {
					catsTemp.tempT2 = (0xFFFFFFFF - catsTemp.tempT1 + 1) + catsTemp.tempT2;
				}
				catsTemp.temp = (2*1000*catsTemp.tempT2) / (NUM_HALF_PERIODS*TEMP_SCALAR_DIV10) - 2731;
				catsTemp.tempTicks = 0;
			}
		}
		LPC_GPIOINT->IO0IntClr |= 1<<2;
	}
}

void TIMER0_IRQHandler(void) {
	if (GPIO_ReadValue(0)>>26 & 0x1) {
		NOTE_PIN_LOW();
	} else {
		NOTE_PIN_HIGH();
	}
	LPC_TIM0->IR |= 1 << 0;
}

int main (void) {
	init_peripherals();
	init_states(&catsState);
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}
	to_mode_stationary(&catsState);
	while (1) {
		if (sw3Ticks && (catsTicks.x1msTicks - sw3Ticks > 1000)) {
			switch (catsState.modeStateNext) {
			case MODE_STATIONARY:
				to_mode_stationary(&catsState);
				break;
			case MODE_FORWARD:
				to_mode_forward(&catsState, &catsTicks, &catsAcc, &catsTemp, ACCX_DISPLAY, TEMP_DISPLAY, UART_DISPLAY);
				break;
			case MODE_REVERSE:
				to_mode_reverse(&catsState, &catsTicks, UART_DISPLAY);
				break;
			}
			sw3Ticks = 0;
		}
		switch (catsState.modeState) {
		case MODE_STATIONARY:
//			printf("%d %d %d\n", (int)acc_interrupt_read(0x06), (int)acc_interrupt_read(0x07), (int)acc_interrupt_read(0x08));
			in_mode_stationary(&catsState);
			break;
		case MODE_FORWARD:
			in_mode_forward(&catsState, &catsTicks, &catsAcc, &catsTemp, ACCX_DISPLAY, TEMP_DISPLAY, UART_DISPLAY);
			break;
		case MODE_REVERSE:
			in_mode_reverse(&catsState, &catsTicks, &luxVal, &catsSpeaker, UART_DISPLAY);
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
