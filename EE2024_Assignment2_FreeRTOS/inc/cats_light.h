/*
 * cats_light.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_LIGHT_H_
#define CATS_LIGHT_H_

#include "cats_header.h"

void lights_init(void);
uint32_t lights_measure(void);
uint16_t lights_to_led(uint32_t light);
float lights_to_beep(uint32_t light);
void lights_start(void);
void lights_stop(void);
uint8_t lights_read_interrupt(void);
void light_clear_interrupt(void);

#endif /* CATS_LIGHT_H_ */
