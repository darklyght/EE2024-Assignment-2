/*
 * cats_uart.c
 *
 *  Created on: Oct 30, 2017
 *      Author: darklyght
 */

#include "cats_uart.h"

/******************************************************************************//*
 * @brief 		Set the UART message
 * @param[in]	UART_DISPLAY is the string to print to
 * @param[in]	string is the string to be displayed
 * @return 		None
 *******************************************************************************/
void set_uart_message(uint8_t* UART_DISPLAY, char* string) {
	strcpy((char*)UART_DISPLAY, "");
	strcat((char*)UART_DISPLAY, string);
}

/******************************************************************************//*
 * @brief 		Generate UART message for forward mode
 * @param[in]	UART_MESSAGE is the string to print to
 * @param[in]	messageCounter is the number of messages sent
 * @param[in]	acc is the acceleration reading
 * @param[in]	temp is the temperature reading
 * @return 		None
 *******************************************************************************/
void uart_display_gen(uint8_t* UART_MESSAGE, uint32_t messageCounter, float acc, int32_t temp) {
	char s[16] = "";
	sprintf(s, "%03d_Temp_", (int)messageCounter);
	strcpy((char*)UART_MESSAGE, s);
	sprintf(s, "%.2f_ACC_", temp / 10.0);
	strcat((char*)UART_MESSAGE, s);
	sprintf(s, "%.2f\r\n", acc);
	strcat((char*)UART_MESSAGE, s);
}
