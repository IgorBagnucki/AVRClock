#include <avr/io.h>
#include "dbg.h"

#define DDR_SPI DDRB
#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DD_SS DDB2
#define P_SS PB2

void SPI_MasterInit(void)
{
	/* Set MOSI, SS and SCK output*/
	DDR_SPI |= (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData)
{

	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)));
}