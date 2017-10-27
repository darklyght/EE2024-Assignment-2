/*
 * init.h
 */

#ifndef INIT_H_
#define INIT_H_

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"

void init_gpio(void);
void init_ssp(void);
void init_i2c(void);

#endif /* INIT_H_ */
