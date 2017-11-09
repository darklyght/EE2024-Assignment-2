/*
 * cats_sdcard.c
 *
 *  Created on: Nov 9, 2017
 */
#include "cats_sdcard.h"

int log(char * logText) {
	return xQueueSend(logQueueHandle, logText, 0)==pdPASS;
}

int blocking_log(char * logText) {
	return xQueueSend(logQueueHandle, logText, portMAX_DELAY)==pdPASS;
}

void logWriterTask(void * pvParameters) {
	logQueueHandle = xCreateQueue(LOG_QUEUE_SIZE, sizeof(char*));
	if (fsStatus!=FR_OK) {
		printf("File System NOT READY!\n");
		vTaskDelete(NULL);
	}
	FIL logFile;
	f_open(&logFile, "log.txt", FA_CREATE_ALWAYS);
	char * logText;
	unsigned int bytesWritten;
	while (1) {
		xQueueReceive(logQueueHandle, &logText, portMAX_DELAY);
		f_write(&logFile, (void *)logText, strlen(logText), &bytesWritten);
		free(logText);
	}
}

void init_sdcard() {
	fsStatus = f_mount(0, &fileSystem);
	if (fsStatus!=FR_OK) {
		printf("File System mounting error!\n");
		return 0;
	}
}
