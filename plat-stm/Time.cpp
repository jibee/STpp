#include "Debug.h"
#include "Time.h"
extern "C" {
#include <FreeRTOS.h>
#include <task.h>
};

using namespace RTOS;

Time::Time(Platform::Timer& t) :
	timer(t) {

	timer
		.setPrescaler(168/2)
		.setOneShot(true);
}

void Time::usleep(int useconds) {
	timer
		.setAutoReload(useconds)
		.enable();
	while(timer);
}

void Time::msleep(int mseconds) {
	vTaskDelay(mseconds);
}
