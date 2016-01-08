#include <CherryPickerBoard.h>
#include <tr1/functional>
#include <Log.h>
#include <Debug.h>
#include <Exti.h>
#include <UsbSerial.h>

int main() {
	bool v = false;
    CherryPickerBoard b;
    UsbSerial usb(b);
	usb << "Hello !" << endl;
	Exti(b.UserButton)
		.enableFalling()
		.disableRising()
		.setTopCB([&](int) {
			usb << "Hello !" << endl;
			b.LedR.setDutyCycle(v ? 100 : 0);
			v = !v;
		})
		.enableIRQ();

	while(1)
		b.time.msleep(10000);
}
