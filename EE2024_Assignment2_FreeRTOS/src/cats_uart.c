/*
 * cats_uart.c
 *
 *  Created on: Oct 30, 2017
 *      Author: darklyght
 */

#include "cats_uart.h"

void set_uart_message(uint8_t* UART_DISPLAY, char* string) {
	strcpy((char*)UART_DISPLAY, "");
	strcat((char*)UART_DISPLAY, string);
}

void uart_display_gen(uint8_t* UART_MESSAGE, uint32_t messageCounter, float acc, int32_t temp) {
	char s[16] = "";
	sprintf(s, "%03d_Temp_", (int)messageCounter);
	strcpy((char*)UART_MESSAGE, s);
	sprintf(s, "%.2f_ACC_", temp / 10.0);
	strcat((char*)UART_MESSAGE, s);
	sprintf(s, "%.2f\r\n", acc);
	strcat((char*)UART_MESSAGE, s);
}
