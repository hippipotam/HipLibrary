/*
 * spi.h
 *
 *  Created on: feb 22, 2017
 *      Author: mike
 * 
 * 		Software SPI.
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdio.h>
#include <inttypes.h>


#define NUM_GPIO_PINS	3

// Port numbers from Olinuxino-NANO documentation 
// (table "The Linux implementations of pins", page 24)
#define GPIO_MOSI	92	// PIN31
#define GPIO_MISO	91	// PIN30
#define GPIO_SCLK	23	// PIN29

// Port directions
#define MOSI_DIR	0	// out
#define MISO_DIR	1	// in
#define SCLK_DIR	0	// out

#define EXPORT		(uint8_t)0x01
#define UNEXPORT	(uint8_t)0x00
#define HIGH		((uint8_t)0x01+'0')
#define LOW			((uint8_t)0x00+'0')
#define SPI_SCLK_LOW_TIME	500000
#define SPI_SCLK_HIGH_TIME	500000

#define ERR_MSG

typedef enum EGPIOPins {
	E_GPIO_MOSI = 0,
	E_GPIO_MISO,
	E_GPIO_SCLK
} EGPIO_pins_t;

/* Initialize SPI */
int SPI_init();
/* Deinitialize SPI */
void SPI_free();
/* Transfer byte */
uint8_t SPI_transfer_byte(uint8_t byte_out);

#endif /* SPI_H_ */
