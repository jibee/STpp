#include <CherryPickerBoard.h>
#include <Log.h>
#include <Uart.h>

int main() {
	CherryPickerBoard b;
	Log::log << "startup" << endl;

	b.Zigbee_UART
		.enable()
		.setMantissa(0x16)
		.setFraction(0xb)
		.enableReceive()
		.enableTransmitter();

	b.Zigbee_TX
		.setPushPull()
		.setAlternate(Gpio::USART4_6)
		.setDirection(Gpio::OUTPUT)
		.setSpeed(Gpio::SPEED_100MHz);

	b.Zigbee_RX
		.setPushPull()
		.setAlternate(Gpio::USART4_6)
		.setDirection(Gpio::INPUT)
		.setSpeed(Gpio::SPEED_100MHz)
		.setResistor(Gpio::PULL_DOWN);
	b.time.msleep(500);

		//Preamble
		b.Zigbee_UART << (char) 0x00;
		//Start
		b.Zigbee_UART << (char) 0x00;
		b.Zigbee_UART << (char) 0xff;
		//Len
		char len = 2;

		b.Zigbee_UART << (char) len;
		b.Zigbee_UART << (char) (~len+1);

		char checksum = 0;
		//TFI
		checksum += 0xd4;
		b.Zigbee_UART << (char) 0xD4;
		//DATA
		checksum += 0x02;
		b.Zigbee_UART << (char) 0x02;

		//Data checksum
		Log::log << "Checksum = " << (int)(~checksum+1);
		b.Zigbee_UART << (char)(~checksum+1);

		//Post-amble
		b.Zigbee_UART << (char) 0x00;

		Log::log << "Done writing serial" << endl;
	while(1) {
		char c;
		b.Zigbee_UART >> c;
		Log::log << (int)c << endl;
	}
}
