#include <CherryPickerBoard.h>
#include <Log.h>
#include <Ax12.h>
#include <Uart.h>

int main() {
	Log::log << "startup" << endl;
	CherryPickerBoard b;
	b.Zigbee_UART
		.enable()
		.setMantissa(0x10e)
		.setFraction(0xc)
		.enableReceive()
		.enableTransmitter();

	b.Zigbee_TX
		.setPushPull()
		.setAlternate(Gpio::USART4_6)
		.setDirection(Gpio::OUTPUT)
		.setSpeed(Gpio::SPEED_100MHz);

	b.Zigbee_RX
		.setAlternate(Gpio::USART4_6)
		.setDirection(Gpio::INPUT)
		.setSpeed(Gpio::SPEED_100MHz)
		.setResistor(Gpio::PULL_UP);

	DmaStream uartDma(DmaStream::DMAController1, DmaStream::S4, DmaStream::C7);
	//Configure the pin
	auto ax12_pin = b.GpioB[10];
	Ax12 ax12_broadcast(b.time, ax12_pin, Uart(3, &uartDma), 0xfe);
	Ax12 ax12_left(b.time, ax12_pin, Uart(3, &uartDma), 0x83);
	Ax12 ax12_right(b.time, ax12_pin, Uart(3, &uartDma), 0x82);
	Ax12 ax12_under(b.time, ax12_pin, Uart(3, &uartDma), 0x81);
	//Enable torque
	ax12_broadcast.setSpeed(0x80);
	ax12_broadcast.enable();

	while(1) {
		char c;
		b.Zigbee_UART << "Going to startup position...";
		//Closed
		{
			ax12_left.goTo(0xb0);
			b.time.msleep(10);
			ax12_right.goTo(0x90);
			b.time.msleep(10);
			b.time.msleep(300);
			ax12_under.goTo(0x330);
			b.time.msleep(10);
		}

		b.Zigbee_UART << "done." << endl;
		b.Zigbee_UART >> c;


		b.Zigbee_UART << "Going to open position...";
		//To go forward
		{
			ax12_under.goTo(0x280);
			b.time.msleep(10);
			b.time.msleep(300);

			int delta = 0x40;
			ax12_left.goTo(0x200+delta);
			b.time.msleep(10);
			ax12_right.goTo(0x200-delta);
			b.time.msleep(10);
		}
		b.Zigbee_UART << "done." << endl;

		b.Zigbee_UART >> c;

		b.Zigbee_UART << "Going to closed position...";
		//To go backward
		{
			//First close braces, then the container
			int delta = 0x15;
			ax12_left.goTo(0x200-delta);
			b.time.msleep(10);
			ax12_right.goTo(0x200+delta);
			b.time.msleep(10);
			ax12_under.goTo(0x280);
			b.time.msleep(10);
		}
		b.Zigbee_UART << "done." << endl;

		b.Zigbee_UART >> c;
	}

	/*
	while(1) {
		ax12_broadcast.goTo(0x200);
		time.msleep(5000);
		ax12_broadcast.goTo(0x330);
		time.msleep(5000);
	}
	*/

	/*
	while(1) {
		ax12_broadcast.goTo(0x200);
		time.msleep(5000);
		ax12_broadcast.goTo(0xb0);
		time.msleep(5000);
	}*/ //bac plus eleve
	/*
	 * Ouvre en cone
	 * ferme symetrique
	 * //recule
	while(1) {
		ax12_left.goTo(0x200);
		time.msleep(10);
		ax12_right.goTo(0x200);
		time.msleep(10);
		ax12_under.goTo(0x200);
		time.msleep(10);

		time.msleep(5000);
		
		ax12_left.goTo(0xb0);
		time.msleep(10);
		ax12_right.goTo(0x90);
		time.msleep(10);
		ax12_under.goTo(0x330);
		time.msleep(10);
		time.msleep(5000);
	}*/
}
