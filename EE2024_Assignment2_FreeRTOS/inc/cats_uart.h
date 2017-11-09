/*
 * cats_uart.h
 *
 *  Created on: Oct 30, 2017
 *      Author: darklyght
 */

#ifndef CATS_UART_H_
#define CATS_UART_H_

#include "cats_header.h"

void set_uart_message(uint8_t* UART_DISPLAY, char* string);
void uart_display_gen(uint8_t* UART_MESSAGE, uint32_t messageCounter, float acc, int32_t temp);

#endif /* CATS_UART_H_ */
