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

STATE catsState;
ACC catsAcc = {0, 0, 0};
TICKS catsTicks = {0, 0, 0};
TEMP_VARS catsTemp = {0, 0, 0, 0};
SPEAKER catsSpeaker = {0, 0, 0};

void SysTick_Handler(void) {
	catsTicks.x1msTicks++;
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
