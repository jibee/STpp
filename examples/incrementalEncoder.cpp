#include <STM32F4.hpp>
#include <tr1/functional>
#include <Log.h>
#include <Exti.h>
#include <Uart.h>
#include <Usb.h>
#include <Watchdog.h>
#include <IncrementalEncoder.h> 
#include <Time.h>

int main() {
    Platform::STM32F4 b;
    RTOS::Time time(b.Tim7);
    Platform::Gpio e9 = b.GpioE[9];
    Platform::Gpio e11 = b.GpioE[11];

    IncrementalEncoder16 codeuse1(e9, e11, b.Tim1);

    while(1){
	time.msleep(200);
	Log::log << "Counter value : " << codeuse1 << endl;
    }

}
