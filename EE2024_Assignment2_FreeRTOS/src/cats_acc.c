/*
 * cats_acc.c
 *
 *  Created on: Oct 22, 2017
 *      Author: darklyght
 */

#include "cats_acc.h"

static int I2CRead(uint8_t addr, uint8_t* buf, uint32_t len) {
	I2C_M_SETUP_Type rxsetup;
	rxsetup.sl_addr7bit = addr;
	rxsetup.tx_data = NULL;	// Get address to read at writing address
	rxsetup.tx_length = 0;
	rxsetup.rx_data = buf;
	rxsetup.rx_length = len;
	rxsetup.retransmissions_max = 3;
	if (I2C_MasterTransferData(LPC_I2C2, &rxsetup, I2C_TRANSFER_POLLING) == SUCCESS){
		return (0);
	} else {
		return (-1);
	}
}

static int I2CWrite(uint8_t addr, uint8_t* buf, uint32_t len) {
	I2C_M_SETUP_Type txsetup;
	txsetup.sl_addr7bit = addr;
	txsetup.tx_data = buf;
	txsetup.tx_length = len;
	txsetup.rx_data = NULL;
	txsetup.rx_length = 0;
	txsetup.retransmissions_max = 3;
	if (I2C_MasterTransferData(LPC_I2C2, &txsetup, I2C_TRANSFER_POLLING) == SUCCESS){
		return (0);
	} else {
		return (-1);
	}
}

void acc_calibrate() {
	uint8_t buf[2];
//	uint32_t i;

	buf[0] = ACC_ADDR_XOFFL;
	buf[1] = 0x00;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_MCTL;
	buf[1] = 0x05;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_XOUT8;
	I2CWrite(ACC_I2C_ADDR, buf, 1);
	I2CRead(ACC_I2C_ADDR, buf, 1);
	printf("Initial acceleration reading: %d\n", (int)buf[0]);
	buf[1] = ((buf[0]^0xFF) + 1)<<1;
	printf("Offset adjustment: %d\n", (int)buf[1]);
	buf[0] = ACC_ADDR_XOFFL;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

//	while (buf[0] != 0x00) {
//		printf("%d\n", (int)buf[0]);
//		buf[1] = ((buf[0]^0xFF) + 1)<<1;
//		printf("%d\n", (int)buf[1]);
//		buf[0] = ACC_ADDR_XOFFL;
//		I2CWrite(ACC_I2C_ADDR, buf, 2);
//		buf[0] = ACC_ADDR_XOUT8;
//		I2CWrite(ACC_I2C_ADDR, buf, 1);
//		I2CRead(ACC_I2C_ADDR, buf, 1);
//	}
}

float acc_measure(void) {
	uint8_t buf[1];

	acc_interrupt_stop();
	buf[0] = ACC_ADDR_XOUT8;
	I2CWrite(ACC_I2C_ADDR, buf, 1);
	I2CRead(ACC_I2C_ADDR, buf, 1);
	acc_interrupt_start();

	return (int8_t)buf[0] / ACC_DIV_MEASURE;
}

void acc_interrupt_init(void) {
	PINSEL_CFG_Type PinCfg;
	uint8_t buf[2];

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<3), 0);
	LPC_GPIOINT->IO0IntEnR |= 1<<3;

//	buf[0] = ACC_ADDR_MCTL;
//	buf[1] = 0x02;
//	I2CWrite(ACC_I2C_ADDR, buf, 2);
//
//	buf[0] = ACC_ADDR_CTL1;
//	buf[1] = 0x31;
//	I2CWrite(ACC_I2C_ADDR, buf, 2);
//
//	buf[0] = ACC_ADDR_CTL2;
//	buf[1] = 0x00;
//	I2CWrite(ACC_I2C_ADDR, buf, 2);
//
//	buf[0] = ACC_ADDR_LDTH;
//	buf[1] = (uint8_t)(ACC_THRESHOLD * ACC_DIV_LEVEL + 1);
//	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_MCTL;
	buf[1] = 0x03;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_CTL1;
	buf[1] = 0x30;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_CTL2;
	buf[1] = 0x00;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_LDTH;
	buf[1] = 0x7F;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_PDTH;
	buf[1] = (uint8_t)(ACC_THRESHOLD * ACC_DIV_LEVEL);
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_PW;
	buf[1] = 0xFF;
	I2CWrite(ACC_I2C_ADDR, buf, 2);
}

void acc_interrupt_start(void) {
	uint8_t buf[2];

//	buf[0] = ACC_ADDR_MCTL;
//	buf[1] = 0x02;
//	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_MCTL;
	buf[1] = 0x03;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	LPC_GPIOINT->IO0IntEnR |= 1<<3;
}

void acc_interrupt_stop(void) {
	uint8_t buf[2];

	buf[0] = ACC_ADDR_MCTL;
	buf[1] = 0x05;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	LPC_GPIOINT->IO0IntEnR &= ~(1<<3);
}

uint8_t acc_interrupt_read(uint8_t address) {
	uint8_t buf[1];

	buf[0] = address;
	I2CWrite(ACC_I2C_ADDR, buf, 1);
	I2CRead(ACC_I2C_ADDR, buf, 1);

	return buf[0];
}

void acc_interrupt_clear(void) {
	uint8_t buf[2];

	buf[0] = ACC_ADDR_INTRST;
	buf[1] = 0x03;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_INTRST;
	buf[1] = 0x00;
	I2CWrite(ACC_I2C_ADDR, buf, 2);
}
