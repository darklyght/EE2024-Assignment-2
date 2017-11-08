/*
 * cats_temp.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_temp.h"

/******************************************************************************//*
 * @brief 		Initialise temperature sensor GPIO
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void temperature_init(void) {
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<2), 0);
}

/******************************************************************************//*
 * @brief 		Read the temperature using interrupt
 * @param[in]	ticks is the tick state of the device
 * @param[in]	temp is the temp state of the device
 * @return 		None
 *******************************************************************************/
void temperature_measure(TICKS* ticks, TEMP* temp) {
	if (!temp->temperatureT1 && !temp->temperatureT2) {
		temp->temperatureT1 = (int)xTaskGetTickCountFromISR();
	} else if (temp->temperatureT1 && !temp->temperatureT2) {
		temp->halfPeriods++;
		if (temp->halfPeriods == TOTAL_HALF_PERIODS) {
			temp->temperatureT2 = (int)xTaskGetTickCountFromISR();
			if (temp->temperatureT2 > temp->temperatureT1) {
				temp->temperatureT2 = temp->temperatureT2 - temp->temperatureT1;
			}
			else {
				temp->temperatureT2 = (0xFFFFFFFF - temp->temperatureT1 + 1) + temp->temperatureT2;
			}
			temp->temperature = (2*1000*temp->temperatureT2) / (TOTAL_HALF_PERIODS*TEMP_DIV) - 2731;
			temp->temperatureT1 = 0;
			temp->temperatureT2 = 0;
			temp->halfPeriods = 0;
		}
	}
}

/******************************************************************************//*
 * @brief 		Unmask temperature sensor GPIO interrupt
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void temperature_start(void) {
	LPC_GPIOINT->IO0IntEnR |= 1<<2;
	LPC_GPIOINT->IO0IntEnF |= 1<<2;
}

/******************************************************************************//*
 * @brief 		Mask temperature sensor GPIO interrupt
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void temperature_stop(void) {
	LPC_GPIOINT->IO0IntEnR &= ~(1<<2);
	LPC_GPIOINT->IO0IntEnF &= ~(1<<2);
}
