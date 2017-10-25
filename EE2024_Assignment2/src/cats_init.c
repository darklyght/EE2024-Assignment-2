/*
 * cats_init.c
 *
 *  Created on: Oct 18, 2017
 *      Author: darklyght
 */

#include "cats_init.h"

#define ACC_I2C_ADDR    0x1D
#define ACC_ADDR_DETSRC 0x0A
#define ACC_ADDR_MCTL   0x16
#define ACC_ADDR_INTRST 0x17
#define ACC_ADDR_CTL1   0x18
#define ACC_ADDR_CTL2   0x19
#define ACC_ADDR_LDTH   0x1A
#define ACC_ADDR_PDTH   0x1B
#define ACC_ADDR_PW     0x1C
#define ACC_ADDR_LT     0x1D
#define ACC_ADDR_TW     0x1E

void acc_interrupt_init(void) {
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<5), 0);
	GPIO_SetDir(0, (1<<3), 0);
	LPC_GPIOINT->IO2IntEnR |= 1<<5;
	LPC_GPIOINT->IO0IntEnR |= 1<<3;

	uint8_t buf[2];

	buf[0] = ACC_ADDR_MCTL;
	buf[1] = 0x02;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_CTL1;
	buf[1] = 0x21;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_CTL2;
	buf[1] = 0x00;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_LDTH;
	buf[1] = 0x0A;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_PDTH;
	buf[1] = 0x0A;
	I2CWrite(ACC_I2C_ADDR, buf, 2);

	buf[0] = ACC_ADDR_PW;
	buf[1] = 0x01;
	I2CWrite(ACC_I2C_ADDR, buf, 2);
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

uint8_t acc_interrupt_read(uint8_t address) {
	uint8_t buf[1];

	buf[0] = address;
	I2CWrite(ACC_I2C_ADDR, buf, 1);
	I2CRead(ACC_I2C_ADDR, buf, 1);

	return buf[0];
}

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

void init_peripherals(void) {
	init_gpio();
	init_ssp();
	init_i2c();
	init_uart();
	acc_interrupt_clear();
	acc_init();
	light_init();
	rgb_init();
	led7seg_init();
	pca9532_init();
	oled_init();
	speaker_init();
	oled_clearScreen(OLED_COLOR_BLACK);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void init_gpio(void) {
	PINSEL_CFG_Type PinCfg;

	// Initialise temperature sensor with interrupt.
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<2), 0);
	LPC_GPIOINT->IO0IntEnR |= 1<<2;
	LPC_GPIOINT->IO0IntEnF |= 1<<2;

	// Initialise SW3 with interrupt
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(2, (1<<10), 0);
	LPC_GPIOINT->IO2IntEnF |= 1<<10;

	// Initialise rotary with interrupt.
	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 25;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<24), 0);
	GPIO_SetDir(0, (1<<25), 0);
	LPC_GPIOINT->IO0IntEnF |= 1<<24;
	LPC_GPIOINT->IO0IntEnF |= 1<<25;
}

void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);
}

void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

void init_uart(void) {
	UART_CFG_Type uartCfg;
	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;
	//pin select for uart3;
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	//supply power & setup working parameters for uart3
	UART_Init(LPC_UART3, &uartCfg);
	//enable transmit for uart3
	UART_TxCmd(LPC_UART3, ENABLE);
}

void init_states(STATE* catsState) {
	catsState->modeState = MODE_STATIONARY;
	catsState->rgbState = BLINK_OFF;
	catsState->accelState = ACCEL_OFF;
	catsState->tempState = TEMP_OFF;
	catsState->obstacleState = OBSTACLE_OFF;
}

void rgb_init (void) {
    GPIO_SetDir(2, 1, 1 );
    GPIO_SetDir(2, (1<<8), 1);
    GPIO_SetDir(2, (1<<1), 1);
}

void speaker_init(void) {
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(2, 1<<6, 1);
	GPIO_SetDir(2, 1<<7, 1);
	GPIO_SetDir(2, 1<<13, 1);
	GPIO_SetDir(0, 1<<26, 1);

	LPC_SC->PCONP |= 1 << 1;      // Power up Timer 0
	LPC_SC->PCLKSEL0 |= 1 << 2;   // Clock for timer = CCLK
	LPC_TIM0->MR0 = 1 << 15;      // Suitable for LED
	LPC_TIM0->MCR |= 1 << 0;      // Interrupt on Match0 compare
	LPC_TIM0->MCR |= 1 << 1;      // Reset timer on Match 0.
	LPC_TIM0->TCR |= 1 << 1;      // Reset Timer0
	LPC_TIM0->TCR &= ~(1 << 1);   // stop resetting the timer.
	LPC_TIM0->TCR &= ~(1 << 0);      // Start timer
	NVIC_EnableIRQ(TIMER0_IRQn);
}
