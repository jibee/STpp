#ifndef _IRREMOTE_H
#define _IRREMOTE_H

#include <Timer.h>
#include <Gpio.h>
#include <Lock.h>

class IRRemote {
	private:
		Platform::Timer& t;
		Platform::Gpio& g;

		Platform::BinarySemaphore dataAvailable;
		Platform::BinarySemaphore dataReleased;
		uint32_t word;
		uint32_t tmpWord;
	public:
		IRRemote(Platform::Timer& t, Platform::Gpio& g);
		int nextRaw();
		int next();
};

#endif /* _IRREMOTE_H */
