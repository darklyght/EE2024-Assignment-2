/*
 * main.c
 */

#include "ff.h"
#include "stdio.h"

int main(void) {
	FATFS fileSystem;
	int status = f_mount(0, &fileSystem);
	if (status!=FR_OK) {
		printf("File System mounting error!\n");
		return 0;
	}
	FIL file;
	char fileName[] = "test.txt";
	status = f_open(&file, (char *)fileName, FA_CREATE_ALWAYS);
	char data[] = "Hello World!";
	unsigned int bytesWritten;
	f_write(&file, (void *)data, sizeof(data), &bytesWritten);
	f_close(&file);
	return 0;
}
