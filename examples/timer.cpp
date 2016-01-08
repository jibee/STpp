#include <Log.h>
#include <STM32F4.hpp>
#include <Time.h>

int main() {
	Platform::STM32F4 b;
	RTOS::Time time(b.Tim7);
	b.Tim4
		.setPrescaler(42)
		.setAutoReload(1000)
		.setOneShot(true)
		.enable();

	Log::log << "Waiting for end of timer 4" << endl;
	while(b.Tim4);
	Log::log << "Timer4 done" << endl;
	while(1)
		time.msleep(2000);
}
