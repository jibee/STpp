#ifndef _HBRIDGE_ST_H
#define _HBRIDGE_ST_H
#include <Gpio.h>
#include <Timer.h>
#include <Pwm.h>

class HBridgeST {
	private:
		Platform::Gpio propa;
		Platform::Gpio propb;
		Pwm pwm;
	public:
		HBridgeST(Platform::Gpio& a, Platform::Gpio& b, Platform::Gpio& pwm, Platform::Timer& tim, int chan);
		void setSpeed(int);
};

#endif /* _HBRIDGE_ST_H */
