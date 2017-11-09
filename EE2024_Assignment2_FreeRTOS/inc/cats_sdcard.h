#ifndef CATS_SDCARD_H_
#define CATS_SDCARD_H_

#define LOG_QUEUE_SIZE 16

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ff.h"
#include <string.h>

xTaskHandle logFileWriterHandle;
xQueueHandle logQueueHandle;

FATFS fileSystem;
int fsStatus;

/**
 * Initializes the SD card and mounts the file system.
 * Must be called before any file operations can occur
 */
void init_sdcard();

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

/**
 * Tries to write "Hello World!" to test.txt
 */
void writeTestFile();

#endif /* CATS_SDCARD_H_ */
