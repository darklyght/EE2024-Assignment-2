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

	// Configure EINT1 interrupt for temperature sensor
	PinCfg.Funcnum = 1;				// Set to EINT1 function
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<11), 0);

	LPC_SC->EXTMODE |= (1<<1);		// Set mode to edge detection
	LPC_SC->EXTPOLAR &= ~(1<<1);	// Set falling edge detection
}

/******************************************************************************//*
 * @brief 		Read the temperature using interrupt
 * @param[in]	ticks is the tick state of the device
 * @param[in]	temp is the temp state of the device
 * @return 		None
 *******************************************************************************/
void temperature_measure(TEMP* temp) {
	if (!temp->temperatureT1 && !temp->temperatureT2) {
		temp->temperatureT1 = (int)xTaskGetTickCountFromISR();
	} else if (temp->temperatureT1 && !temp->temperatureT2) {
		temp->halfPeriods++;
		if (temp->halfPeriods == TOTAL_HALF_PERIODS / 2) {
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
	NVIC_EnableIRQ(EINT1_IRQn);
}

/******************************************************************************//*
 * @brief 		Mask temperature sensor GPIO interrupt
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void temperature_stop(void) {
	NVIC_DisableIRQ(EINT1_IRQn);
}
