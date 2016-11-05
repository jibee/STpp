#ifndef _SPI_H
#define _SPI_H

#include <stm32f4xx.h>
#include "OStream.h"
#include "IStream.h"
namespace Platform
{
class Gpio;
class DmaStream;
/** SPI bus control
*/
class Spi
{
    private:
	int n;
	SPI_TypeDef *base;
	Spi& rxDma(bool enable=true);
	Spi& txDma(bool enable=true);
	DmaStream *txdma;

	/** Shared initialisation routine
	 * */
	void init();
    public:
	/** Constructor
	 *
	 * @param n the SPI port to use; acceptable values range from 1 to 3
	 * */

	Spi(int n);
	/** Constructor
	 *
	 * @param n the SPI port to use; acceptable values range from 1 to 3
	 * @param stream DMA stream to use for write operations
	 * */
	Spi(int n, DmaStream* stream);
	/** Enables the SPI device (including its internal clock)
	 *
	 * @param v set to true to enable the device, set to false to disable it.
	 *
	 * @todo a specific shutdown procedure ought to be followed. set section 27.3.8
	 * */
	Spi& enable(bool v=true);

	/** Sets the baud rate relative to the Peripheral Clock (PCLK)
	 *
	 * The baud rate by taking the PCLK and dividing it by a certain power of 2.
	 *
	 * @param d the exponent of 2 by which the clock will be divided by.
	 *
	 * Examples:
	 * 0: PCLK/2
	 * 1: PCLK/4
	 * 2: PCLK/8
	 * ...
	 * 7: PCLK/256
	 * */
	Spi& setDivisorPow2(int d);
	/** Sets the SPI device in master mode - ie transmissions will use our own
	 * clock */
	Spi& master();
	/** Sets the SPI device in slave mode - ie transmissions will use an externally
	 * generated clock
	 */
	Spi& slave();
	/**
	 * Set the data clock phase.
	 *
	 * @todo implementation
	 * */
	Spi& setDataClockPhase(bool second);
	/**
	 * Sets the data clock polarity
	 *
	 * @todo implementation
	 * */
	Spi& setClockPolarity(bool up);
	/**
	 * Sets the data byte order
	 *
	 * @param lsbFirst true to send the Least Significant Bit first; false to send the Most Significant Bit first.
	 *
	 * */
	Spi& setLsbFirst(bool lsbFirst);
	/** Send a single byte of data through the SPI bus.
	 *
	 * @param c character to be sent.
	 * */

	Spi& send(char c);
	/** Send a series of bytes through the SPI bus.
	 *
	 * @param s string to be sent
	 * @param len number of bytes to be sent
	 *
	 * @warning when DMA is not activated this will return once
	 * all the data has been clocked out. With DMA activated this will
	 * return immediately; however care must be taken not to free the
	 * data string before the transmission actually completed.
	 *
	 *
	 * */
	Spi& send(char* s, int len);
	/** Configures a GPIO for use as IO port for this SPI device
	 *
	 * @param io the IO port to use.
	 * @warning this function does not check the usefulness of the GPIO
	 * */
	Spi& configGpio(Gpio& io);
	/** Set the DMA stream to use for TX operations */
	Spi& setTxDma(DmaStream* stream);
};
}
#endif /* _SPI_H */
