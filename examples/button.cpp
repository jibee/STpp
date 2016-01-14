#include <DiscoveryBoard.hpp>
#include <tr1/functional>
#include <Log.h>
#include <Debug.h>
#include <Exti.h>
#include <UsbSerial.h>
#include <Pwm.h>
#include <Time.h>


/**
 * Demonstrates the use of interrupts and C++11 lambdas to respond to an external stimulus.
 * 
 * When the user presses the button the red led will be toggled on and off.
 */
int main() {
    bool v = false;
    DiscoveryBoard b;
    UsbSerial usb(b);
    Pwm<GeneralPurposeTimer<uint16_t, 4>> LedR(b.g_LedR, b.Tim4, 3);
    RTOS::Time time(b.Tim7);
    usb << "Hello !" << endl;
    Exti(b.UserButton)
	.enableFalling()
	.disableRising()
	.setTopCB([&](int) {
		usb << "Hello !" << endl;
		LedR.setDutyCycle(v ? 100 : 0);
		v = !v;
		})
    .enableIRQ();

    while(1)
	time.msleep(10000);
}
