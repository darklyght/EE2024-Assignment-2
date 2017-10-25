/*
 * cats_init.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_INIT_H_
#define CATS_INIT_H_

#include "cats_header.h"
#include "cats_timer.h"
#include "cats_sw3.h"
#include "cats_acc.h"
#include "cats_temp.h"
#include "cats_light.h"
#include "cats_rgb.h"
#include "cats_amp.h"

void init_peripherals(void);
void init_ssp(void);
void init_i2c(void);
void init_uart(void);

#endif /* CATS_INIT_H_ */
