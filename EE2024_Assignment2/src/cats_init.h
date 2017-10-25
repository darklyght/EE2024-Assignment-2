/*
 * cats_init.h
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#ifndef CATS_INIT_H_
#define CATS_INIT_H_

#include "header.h"

void acc_interrupt_init(void);
void acc_interrupt_clear(void);
uint8_t acc_interrupt_read(uint8_t address);
static int I2CRead(uint8_t addr, uint8_t* buf, uint32_t len);
static int I2CWrite(uint8_t addr, uint8_t* buf, uint32_t len);
void init_peripherals(void);
void init_gpio(void);
void init_ssp(void);
void init_i2c(void);
void init_uart(void);
void init_states(STATE* catsState);
void rgb_init (void);
void speaker_init(void);

#endif /* CATS_INIT_H_ */
