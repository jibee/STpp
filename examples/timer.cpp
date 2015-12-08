#include <tr1/functional>
#include <Log.h>
#include <Ax12.h>
#include <Exti.h>
#include <Uart.h>
#include <Usb.h>
#include <Watchdog.h>
#include <Board.h>

int main() {
    Board b;
	b.Tim4
		.setPrescaler(42)
		.setAutoReload(1000)
		.setOneShot(true)
		.enable();

	log << "Waiting for end of timer 4" << endl;
	while(b.Tim4);
	log << "Timer4 done" << endl;
	while(1)
		b.time.msleep(2000);
}
