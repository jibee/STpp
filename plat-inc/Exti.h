#ifndef _EXTI_H
#define _EXTI_H
#include "Gpio.h"
#include <tr1/functional>
namespace Platform
{
/** Callback for external interrupts */
void exti_handler();

/** Handler for monitoring GPIO trigger events.
 *
 * Note the hardware limitations means you cannot have two interrupts monitoring
 * 2 GPIOS of the same number but in different ports.
 *
 * */
class Exti
{
    public:
	/** GPIO ports available
	 *
	 * @smell this is a duplicate of a similar declaration in Gpio.h
	 */
	enum Port {
	    A = 0,
	    B = 1,
	    C = 2,
	    D = 3,
	    E = 4,
	    F = 5,
	    G = 6,
	    H = 7,
	    I = 8,
	};
	/** Callback for external events
	 *
	 * @param int interrupt line number - from 0 to 23. 0 to 15 actually
	 * reflect GPIO-driven interrupts
	 * */
	typedef std::tr1::function<void(int)> Callback;
    private:
	/** Sets the GPIO port
	 *
	 * @param port GPIO Port number
	 */
	void setGpioPort(int port);
	/**
	 * GPIO pin number and EXTI channel number
	 * */
	int chan;
	/** GPIO Port being listened to
	 * */
	enum Port port;
	/** Obtains the IRQ number relative to this channel
	 * */
	int irq_n();
	/** Array of call back functions */
	static Callback top_cb[15];
	/** @smell unused */
	static Callback bottom_cb[15];
	/** Calls the n-th callback function
	 * */
	static void callTopCB(int nr);
	/** @smell unused */
	static void callBottomCB(int nr);
	/** general handler for EXT interrupts */
	friend void exti_handler();
    public:
	//typedef function<void(int)> Callback;
	/**
	 * Constructor
	 *
	 * @param c pin number
	 * @param p GPIO port
	 *
	 * */
	Exti(int c, Port p);
	/**
	 * Constructor
	 *
	 * @param p GPIO pin to monitor
	 * */
	Exti(const Gpio& p);
	/** Enables the interrupt generation for this external interrupt
	 *
	 */
	Exti& enableIRQ();
	/**
	 * Disables the interrupt generation for this external interrupt.
	 *
	 * @smell when a single actual IRQ is shared by many Exti this disables all such
	 * interrupts at once.
	 */
	Exti& disableIRQ();
	/** Enables interrupt generation on a rising edge */
	Exti& enableRising();
	/** Disables interrupt generation on a rising edge */
	Exti& disableRising();
	/** Enables interrupt generation on a rising edge */
	Exti& enableFalling();
	/** Disables interrupt generation on a rising edge */
	Exti& disableFalling();
	/** Sets the callback function to call when an interrupt is generated */
	Exti& setTopCB(Callback cb);
	/** @smell unused */
	Exti& setBottomCB(Callback cb);
};
}

#endif /* _EXTI_H */
