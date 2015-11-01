#ifndef _UART_H
#define _UART_H

#include "Gpio.h"
#include <stm32f4xx.h>
#include "OStream.h"
#include "IStream.h"
#include "Dma.h"
#include "Lock.h"

namespace Platform
{
/** 
Universal Asynchronous Receiver/Transmitter

This class provides an input and an output stream into a serial port.

Characters we receive are handled through interrupts and buffered until
obtained through userspace.

*/
class Uart : public OStream, public IStream {
    private:
	/** UART number. */
	int number;
	/** Control registers mapping */
	volatile USART_TypeDef *base;
	/** Initialise the UART */
	void init();
	/** Dma stream used for write operations */
	DmaStream* dma;
	Mutex lock;
    public:
/** Constructor
@param n UART number - from 1 to 6
*/
	Uart(int n);
/** Constructor
@param n UART number, from 1 to 6 
@param dma Dma controller to use to accelerate writes into the UART
*/
	Uart(int n, DmaStream* dma);
/** Configures a IO pin to be used by the UART
@warning this only configures the IO muxes to internally connect to the UART
function. There are no checks that this configuration makes any sense.

@param p the Gpio to configure
*/
	Uart& configGpio(Gpio& p);
/** Configures the pair of GPIO to be used as RX and TX pins

@param rx the reception pin
@param tx the tranmission pin
@warning this only configures the IO muxes to internally connect to the UART
function. There are no checks that this configuration makes any sense.
*/
	Uart& configGpio(Gpio& rx, Gpio& tx);
/** Obtains the next received character; blocks until a character is received

@return the received character
*/
	char waitForNext();
/** Set the mantissa (ie the integral part) of the clock to baud diviser 
*/
	Uart& setMantissa(int);
/** Set the fraction part of the clock to baud diviser */
	Uart& setFraction(int);
	Uart& enableReceive();
	Uart& disableReceive();
	Uart& enableTransmitter();
	Uart& disableTransmitter();
	Uart& enable();
	Uart& txDma(bool en);
	Uart& disable();
	Uart& sendBreak();
	Uart& setHalfDuplex(bool);
    public:
	virtual Uart& put(char);
	virtual Uart& put(char*, int);
	virtual Uart& endl();
    public:
	virtual int get();
	virtual bool available();
	virtual void wait();
};
}
#endif /* _UART_H */
