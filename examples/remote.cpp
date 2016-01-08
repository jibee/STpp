#include <STM32F4.hpp>
#include <tr1/functional>
#include <UsbSerial.h>
#include <RPC.h>

static RPC rpc;

int main() {
	Platform::STM32F4_WithUSB b;
	Platform::UsbSerial usb(b);

	rpc.setStream(&usb, &usb);
	rpc.registerClass(0x00, [&usb](int len, char *msg) {
		for(int i=0; i<len; ++i) {
			usb << (char)msg[i];
		}
	});
	rpc.run();

	return 0;
}
