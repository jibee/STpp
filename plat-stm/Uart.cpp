#include <FreeRTOS.h>
#include <queue.h>
#include <Uart.h>
#include <Log.h>
#include <Irq.h>
#include <stm32f4xx.h>


/** "pre-declare" the interrupt handlers as C-levels so they can be insered
into the interrupt vectors */

extern "C" {
	void USART1_IRQHandler(void);
	void USART2_IRQHandler(void);
	void USART3_IRQHandler(void);
	void UART4_IRQHandler(void);
	void UART5_IRQHandler(void);
	void USART6_IRQHandler(void);
};

using namespace Platform;

/// Reception queue.
static xQueueHandle uart_queue_rx[7];
//static xQueueHandle uart_queue_tx[7];

/** General handler for receive interrupts.
*/
static void irq_handler(volatile USART_TypeDef* b, int i) {
	if(b->SR & USART_SR_RXNE) {
		char val = b->DR;
		long v;
		xQueueSendFromISR(uart_queue_rx[i], &val, &v);
		portEND_SWITCHING_ISR(v);
	}
}
/** actual handler for UART1 */
void USART1_IRQHandler() {
	irq_handler(USART1, 1);
}

/** actual handler for UART2 */
void USART2_IRQHandler() {
	irq_handler(USART2, 2);
}

/** actual handler for UART3 */
void USART3_IRQHandler() {
	irq_handler(USART3, 3);
}

/** actual handler for UART4 */
void UART4_IRQHandler() {
	irq_handler(UART4, 4);
}

/** actual handler for UART5 */
void UART5_IRQHandler() {
	irq_handler(UART5, 5);
}

/** actual handler for UART6 */
void USART6_IRQHandler() {
	irq_handler(USART6, 6);
}

void Uart::init() {
	int n = number;
	switch(n) {
		case 1:
			RCC->APB2ENR |= 1<<4;
			base = (volatile USART_TypeDef*)0x40011000;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			RCC->APB1ENR |= 1<<(15+n);
			base = (volatile USART_TypeDef*)(0x40004400+(n-2)*0x400);
			break;
		case 6:
			RCC->APB2ENR |= 1<<5;
			base = (volatile USART_TypeDef*)0x40011400;
			break;
	}
	uart_queue_rx[n] = xQueueCreate(32, 1);
}

Uart::Uart(int n) :
	number(n), dma(NULL) {
	init();
}

Uart::Uart(int n, DmaStream* dma) :
	number(n), dma(dma) {
	init();
	dma->
		peripheralFixed(true)
		.memoryFixed(false)
		.fifo(true)
		.setDirection(DmaStream::M2P)
		.peripheralControlled(false)
		.setPeripheral(&(base->DR));
}

Uart& Uart::configGpio(Gpio& p) {
	Gpio::AF af;

	if(number < 4)
		af = Gpio::USART1_3;
	else
		af = Gpio::USART4_6;

	p.setAlternate(af);
	p.setSpeed(Gpio::SPEED_100MHz);
	p.setPushPull();

	//Perhaps control direction... one day...
	return *this;
}

Uart& Uart::configGpio(Gpio& rx, Gpio& tx) {
	configGpio(rx);
	rx.setDirection(Gpio::INPUT);
	configGpio(tx);
	tx.setDirection(Gpio::OUTPUT);
	return *this;
}

Uart& Uart::put(char* s, int l) {
	if(!dma) {
		for(int i=0; i<l; ++i)
			put(s[i]);
		return *this;
	}
	AutoLock m(lock);
	txDma(true);
	base->SR  &= ~USART_SR_TC;
	dma->
		numberOfData(l)
		.setMemory(s)
		.enable()
		.wait();

	return *this;
}

Uart& Uart::put(char c) {
	if(dma) {
		static char dma_c __attribute((section("dma")));
		dma_c = c;
		put(&dma_c, 1);
		while(! (base->SR & (1<<7)));
		return *this;
	}
	AutoLock m(lock);
	while(! (base->SR & (1<<7)));
	base->DR = c;
	while(! (base->SR & (1<<7)));

	return *this;
}

Uart& Uart::setMantissa(int m) {
	m&=0xfff;
	base->BRR = (base->BRR & ~(0xfff<<4)) | (m<<4);
	return *this;
}

Uart& Uart::setFraction(int f) {
	f&=0xf;
	base->BRR = (base->BRR & ~(0xf)) | f;
	return *this;
}

Uart& Uart::enableReceive() {
	base->CR1 |= 1<<2;
	return *this;
}

Uart& Uart::disableReceive() {
	base->CR1 &= ~(1<<2);
	return *this;
}

Uart& Uart::enableTransmitter() {
	base->CR1 |= 1<<3;
	return *this;
}

Uart& Uart::disableTransmitter() {
	//Wait for transmission complete
	while(! (base->SR & (1<<6)));

	base->CR1 &= ~(1<<3);
	return *this;
}

Uart& Uart::enable() {
#if 1
	//Enable IRQ
	//On TX
	//base->CR1 |= 1<<6;
	//On RX
	base->CR1 |= 1<<5;

	int irqnr = 0;
	if(number < 4)
		irqnr = USART1_IRQn + number - 1;
	else if(number < 6)
		irqnr = UART4_IRQn + number - 4;
	else if(number == 6)
		irqnr = USART6_IRQn;
	Irq(irqnr)
		.setPriority(230)
		.enable();
#endif

	base->CR1 |= 1<<13;
	return *this;
}

Uart& Uart::disable() {
	base->CR1 &= ~(1<<13);
	return *this;
}

char Uart::waitForNext() {
	char c;
	xQueueReceive(uart_queue_rx[number], &c, portMAX_DELAY);
	return c;
}

Uart& Uart::sendBreak() {
	base->CR1 |= 1;
	while( base->CR1 & 1);
	return *this;
}

Uart& Uart::setHalfDuplex(bool b) {
	if(b) {
		base->CR3 |= 1<<3;
	} else {
		base->CR3 &= ~(1<<3);
	}
	return *this;
}

Uart& Uart::endl() {
	static char eol[] __attribute((section("dma"))) = "\r\n";
	put(eol, sizeof(eol)-1);
	return *this;
}

int Uart::get() {
	char r;
	int res = xQueueReceive(uart_queue_rx[number], &r, 0);
	if(!res)
		return -1;
	return r;
}

bool Uart::available() {
	return !xQueueIsQueueEmptyFromISR(uart_queue_rx[number]);
}

void Uart::wait() {
	char c;
	xQueuePeek(uart_queue_rx[number], &c, portMAX_DELAY);
}

Uart& Uart::txDma(bool en) {
	if(en)
		base->CR3 |= USART_CR3_DMAT;
	else
		base->CR3 &= ~USART_CR3_DMAT;
	return *this;
}
