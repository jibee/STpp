#include <STM32F4.hpp>
#include <Log.h>
#include <IRRemote.h>
#include <Debug.h>

int main() {
    Platform::STM32F4 b;
    auto pin1 = b.GpioE[7];
    IRRemote remote(b.Tim12, pin1);
    while(true)
	debug << remote.next();
}
