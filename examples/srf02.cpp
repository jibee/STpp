#include <CherryPickerBoard.h>
#include <Log.h>
#include <Debug.h>

int main() {
	CherryPickerBoard b;
	Log::log << "startup" << endl;

	//115200 = 38400 * 3 = 9600 * 12
	//
	//38400
	//0x43b
	//
	//9600
	//0x10ec
	b.ExternalUart
		.enable()
		.setMantissa(0x10e)
		.setFraction(0xc)
		.enableReceive()
		.enableTransmitter()
		.configGpio(b.External_RX, b.External_TX);


	b.External_TX.setDirection(Gpio::OUTPUT);
	b.External_RX
		.setResistor(Gpio::PULL_UP);

	char addr = 1;
	while(1) {
		static char pulse[] __attribute__((section("dma"))) = "\x01\x51";
		pulse[0]=addr;
		b.ExternalUart.put(pulse, sizeof(pulse));
		b.time.msleep(300);

		static char result[] __attribute__((section("dma"))) = "\x01\x5e";
		result[0]=addr;
		b.ExternalUart.put(result, sizeof(result));
		char c1,c2;
		b.ExternalUart >> c1 >> c2;
		debug << (int)(c1 << 8 | c2) << endl;

		b.time.msleep(500);
	}
}
