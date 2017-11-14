/*
 * cats_task_commons.h
 *
 *  Created on: Nov 14, 2017
 *      Author: Ang Zhi Yuan
 */

#ifndef CATS_TASK_COMMONS_H_
#define CATS_TASK_COMMONS_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

xTaskHandle xSwitchModeTaskHandle;
xTaskHandle xModeTaskHandle;
xTaskHandle xRGBBlinkHandle;
xTaskHandle xAmpBeepHandle;
xTaskHandle xAmpVolumeHandle;

xSemaphoreHandle switchModeSemaphore;
xSemaphoreHandle ampVolumeSemaphore;

#define giveAndYield( xSemaphore ) {																		\
		portBASE_TYPE higherPriorityWoken = pdFALSE;						\
		xSemaphoreGiveFromISR(xSemaphore, &higherPriorityWoken);	\
		portYIELD_FROM_ISR(higherPriorityWoken);							\
}

#endif /* CATS_TASK_COMMONS_H_ */
