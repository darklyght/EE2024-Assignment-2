/*
 * cats_timer.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_timer.h"

void timer_init(void) {
	LPC_SC->PCONP |= (1<<2);		// Power up Timer0
	LPC_SC->PCLKSEL0 |= (1<<4);		// CCLK
}

void timer_start_forward(void) {
	LPC_TIM1->TCR &= ~(1<<0);		// Stop Timer1
	LPC_TIM1->MR0 = 0x01FCA055;		// Match0 333ms
	LPC_TIM1->MCR |= (1<<0);		// Interrupt on Match0
	LPC_TIM1->MCR &= ~(1<<1);		// Don't reset on Match0
	LPC_TIM1->MCR &= ~(1<<2);		// Don't stop on Match0
	LPC_TIM1->MR1 = 0x03F940AA;		// Match1 666ms
	LPC_TIM1->MCR |= (1<<3);		// Interrupt on Match1
	LPC_TIM1->MCR &= ~(1<<4);		// Don't reset on Match1
	LPC_TIM1->MCR &= ~(1<<5);		// Don't stop on Match1
	LPC_TIM1->MR2 = 0x05F5E100;		// Match2 1s
	LPC_TIM1->MCR |= (1<<6);		// Interrupt on Match2
	LPC_TIM1->MCR |= (1<<7);		// Reset on Match2
	LPC_TIM1->MCR &= ~(1<<8);		// Don't stop on Match1
	LPC_TIM1->MR3 = 0x0;			// Match3 off
	LPC_TIM1->MCR &= ~(1<<9);		// Don't interrupt on Match3
	LPC_TIM1->MCR &= ~(1<<10);		// Don't reset on Match3
	LPC_TIM1->MCR &= ~(1<<11);		// Don't stop on Match3
	NVIC_EnableIRQ(TIMER1_IRQn);
	LPC_TIM1->TCR &= ~(1<<0);		// Stop Timer1
	LPC_TIM1->TCR |= (1<<1);		// Reset Timer1
	LPC_TIM1->TCR &= ~(1<<1);		// Clear reset
	LPC_TIM1->TCR |= (1<<0);		// Start Timer1
}

void timer_start_reverse(uint8_t frequency) {
	LPC_TIM1->TCR &= ~(1<<0);		// Stop Timer1
	switch(frequency) {
	case 0x1:
		LPC_TIM1->MR0 = 0x0;			// Match0 off
		LPC_TIM1->MCR &= ~(1<<0);		// Don't interrupt on Match0
		LPC_TIM1->MCR &= ~(1<<1);		// Don't reset on Match0
		LPC_TIM1->MCR &= ~(1<<2);		// Don't stop on Match0
		LPC_TIM1->MR1 = 0x0;			// Match1 off
		LPC_TIM1->MCR &= ~(1<<3);		// Don't interrupt on Match1
		LPC_TIM1->MCR &= ~(1<<4);		// Don't reset on Match1
		LPC_TIM1->MCR &= ~(1<<5);		// Don't stop on Match1
		LPC_TIM1->MR2 = 0x0;			// Match2 off
		LPC_TIM1->MCR &= ~(1<<6);		// Don't interrupt on Match2
		LPC_TIM1->MCR &= ~(1<<7);		// Don't reset on Match2
		LPC_TIM1->MCR &= ~(1<<8);		// Don't stop on Match1
		LPC_TIM1->MR3 = 0x05F5E100;		// Match3 1s
		LPC_TIM1->MCR |= (1<<9);		// Interrupt on Match3
		LPC_TIM1->MCR |= (1<<10);		// Reset on Match3
		LPC_TIM1->MCR &= ~(1<<11);		// Don't stop on Match3
		break;
	case 0x2:
		LPC_TIM1->MR0 = 0x02FAF080;		// Match0 500ms
		LPC_TIM1->MCR |= (1<<0);		// Interrupt on Match0
		LPC_TIM1->MCR &= ~(1<<1);		// Don't reset on Match0
		LPC_TIM1->MCR &= ~(1<<2);		// Don't stop on Match0
		LPC_TIM1->MR1 = 0x0;			// Match1 off
		LPC_TIM1->MCR &= ~(1<<3);		// Don't interrupt on Match1
		LPC_TIM1->MCR &= ~(1<<4);		// Don't reset on Match1
		LPC_TIM1->MCR &= ~(1<<5);		// Don't stop on Match1
		LPC_TIM1->MR2 = 0x0;			// Match2 off
		LPC_TIM1->MCR &= ~(1<<6);		// Don't interrupt on Match2
		LPC_TIM1->MCR &= ~(1<<7);		// Don't reset on Match2
		LPC_TIM1->MCR &= ~(1<<8);		// Don't stop on Match1
		LPC_TIM1->MR3 = 0x05F5E100;		// Match3 1s
		LPC_TIM1->MCR |= (1<<9);		// Interrupt on Match3
		LPC_TIM1->MCR |= (1<<10);		// Reset on Match3
		LPC_TIM1->MCR &= ~(1<<11);		// Don't stop on Match3
		break;
	case 0x3:
		LPC_TIM1->MR0 = 0x01FCA055;		// Match0 333ms
		LPC_TIM1->MCR |= (1<<0);		// Interrupt on Match0
		LPC_TIM1->MCR &= ~(1<<1);		// Don't reset on Match0
		LPC_TIM1->MCR &= ~(1<<2);		// Don't stop on Match0
		LPC_TIM1->MR1 = 0x03F940AA;		// Match1 666ms
		LPC_TIM1->MCR |= (1<<3);		// Interrupt on Match1
		LPC_TIM1->MCR &= ~(1<<4);		// Don't reset on Match1
		LPC_TIM1->MCR &= ~(1<<5);		// Don't stop on Match1
		LPC_TIM1->MR2 = 0x0;			// Match2 off
		LPC_TIM1->MCR &= ~(1<<6);		// Don't interrupt on Match2
		LPC_TIM1->MCR &= ~(1<<7);		// Don't reset on Match2
		LPC_TIM1->MCR &= ~(1<<8);		// Don't stop on Match1
		LPC_TIM1->MR3 = 0x05F5E100;		// Match3 1s
		LPC_TIM1->MCR |= (1<<9);		// Interrupt on Match3
		LPC_TIM1->MCR |= (1<<10);		// Reset on Match3
		LPC_TIM1->MCR &= ~(1<<11);		// Don't stop on Match3
		break;
	case 0x4:
		LPC_TIM1->MR0 = 0x017D7840;		// Match0 250ms
		LPC_TIM1->MCR |= (1<<0);		// Interrupt on Match0
		LPC_TIM1->MCR &= ~(1<<1);		// Don't reset on Match0
		LPC_TIM1->MCR &= ~(1<<2);		// Don't stop on Match0
		LPC_TIM1->MR1 = 0x02FAF080;		// Match1 500ms
		LPC_TIM1->MCR |= (1<<3);		// Interrupt on Match1
		LPC_TIM1->MCR &= ~(1<<4);		// Don't reset on Match1
		LPC_TIM1->MCR &= ~(1<<5);		// Don't stop on Match1
		LPC_TIM1->MR2 = 0x047868C0;		// Match2 750ms
		LPC_TIM1->MCR |= (1<<6);		// Interrupt on Match2
		LPC_TIM1->MCR &= ~(1<<7);		// Don't reset on Match2
		LPC_TIM1->MCR &= ~(1<<8);		// Don't stop on Match1
		LPC_TIM1->MR3 = 0x05F5E100;		// Match3 1s
		LPC_TIM1->MCR |= (1<<9);		// Interrupt on Match3
		LPC_TIM1->MCR |= (1<<10);		// Reset on Match3
		LPC_TIM1->MCR &= ~(1<<11);		// Don't stop on Match3
		break;
	}
	NVIC_EnableIRQ(TIMER1_IRQn);
	LPC_TIM1->TCR &= ~(1<<0);		// Stop Timer1
	LPC_TIM1->TCR |= (1<<1);		// Reset Timer1
	LPC_TIM1->TCR &= ~(1<<1);		// Clear reset
	LPC_TIM1->TCR |= (1<<0);		// Start Timer1
}

void timer_stop(void) {
	LPC_TIM1->TCR &= ~(1<<0);		// Stop Timer1
}
