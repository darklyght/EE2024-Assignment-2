#ifndef CATS_SDCARD_H_
#define CATS_SDCARD_H_

#define LOG_QUEUE_SIZE 16

#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

xTaskHandle logFileWriterHandle;
QueueHandle_t logQueueHandle;

FATFS fileSystem;
int fsStatus;

/**
 * Tries to queue the specified string to the log writer
 * String will be deleted after writing
 * Returns 1 if successful, 0 otherwise
 */
int log(char * logText);

/**
 * Queues the specified string to the log writer
 * Blocks if the queue is full
 * String will be deleted after writing
 * Returns 1 if successful, 0 otherwise
 */
int blocking_log(char * logText);

/**
 * Task to handle log writing
 */
void logWriterTask(void * pvParameters);



#endif /* CATS_SDCARD_H_ */
