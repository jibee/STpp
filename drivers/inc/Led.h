#ifndef _LED_H
#define _LED_H

#include "Gpio.h"
#include "Lock.h"

class Led {
	private:
		Platform::Mutex _m;
		Platform::Gpio _p;
		bool _rev;
	public:
		Led(Platform::Gpio p, bool rev=false);
		Led& on();
		Led& off();
		Led& toggle();
};

#endif /* _LED_H */
