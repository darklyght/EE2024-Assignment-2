/*
 * cats_init.h
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#ifndef CATS_INIT_H_
#define CATS_INIT_H_

#include "header.h"

void init_peripherals(void);
void init_gpio(void);
void init_ssp(void);
void init_i2c(void);
void init_uart(void);
void init_states(STATE* catsState);

#endif /* CATS_INIT_H_ */
