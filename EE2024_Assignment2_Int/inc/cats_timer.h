/*
 * cats_timer.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_TIMER_H_
#define CATS_TIMER_H_

#include "cats_header.h"

void timer_init(void);
void timer_start_forward(void);
void timer_start_reverse(uint8_t frequency);
void timer_stop(void);

#endif /* CATS_TIMER_H_ */
