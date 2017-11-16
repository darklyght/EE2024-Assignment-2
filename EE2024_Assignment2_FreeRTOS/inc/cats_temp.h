/*
 * cats_temp.h
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#ifndef CATS_TEMP_H_
#define CATS_TEMP_H_

#include "cats_header.h"

void temperature_init(void);
void temperature_measure(TEMP* temp);
void temperature_start(void);
void temperature_stop(void);

#endif /* CATS_TEMP_H_ */
