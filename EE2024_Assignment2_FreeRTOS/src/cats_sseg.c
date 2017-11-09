/*
 * cats_sseg.c
 *
 *  Created on: Oct 28, 2017
 *      Author: darklyght
 */

#include "cats_sseg.h"

#define LED7_CS_OFF() GPIO_SetValue(2, (1<<2))
#define LED7_CS_ON()  GPIO_ClearValue(2, (1<<2))

static uint8_t chars[] = {
        /* '-', '.' */
        0xFB, 0xDF, 0xFF,
        /* digits 0 - 9 */
        0x24, 0xAF, 0xE0, 0xA2, 0x2B, 0x32, 0x30, 0xA7, 0x20, 0x22,
        /* ':' to '@' are invalid */
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        /* A to J */
        0x21, 0x20, 0x74, 0x24, 0x70, 0x71, 0x10, 0x29, 0x8F, 0xAC,
        /* K to T */
        0xFF, 0x7C,  0xFF, 0xB9, 0x04, 0x61, 0x03, 0xF9, 0x12, 0x78,
        /* U to Z */
        0x2C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        /* '[' to '`' */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF,
        /* a to j */
        0x21, 0x38, 0xF8, 0xA8, 0x70, 0x71, 0x02, 0x39, 0x8F, 0xAC,
        /* k to t */
        0xFF, 0x7C,  0xFF, 0xB9, 0xB8, 0x61, 0x03, 0xF9, 0x12, 0x78,
        /* u to z */
        0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        /* { to '|' */
        0xFF, 0x7D,

};

/******************************************************************************//*
 * @brief 		Initialise 7-segment display
 * @param[in]	None
 * @return 		None
 *******************************************************************************/
void sseg_init (void) {
	GPIO_SetDir(2, (1<<2), 1);
    LED7_CS_OFF();
}

/******************************************************************************//*
 * @brief 		Set the character on the 7-segment display
 * @param[in]	ch is the character to display
 * @param[in]	rawMode if false turn on 7-segment as per character specified in ch
 * @return 		None
 *******************************************************************************/
void sseg_set(uint8_t ch, uint32_t rawMode) {
    uint8_t val = 0xff;
    SSP_DATA_SETUP_Type xferConfig;

    if (ch >= '-' && ch <= '|') {
        val = chars[ch-'-'];
    }
    if (rawMode) {
        val = ch;
    }

	xferConfig.tx_data = &val;
	xferConfig.rx_data = NULL;
	xferConfig.length = 1;

    LED7_CS_ON();
    SSP_ReadWrite(LPC_SSP1, &xferConfig, SSP_TRANSFER_POLLING);
    LED7_CS_OFF();
}
