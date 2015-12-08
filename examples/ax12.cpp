#include <Board.h>
#include <Log.h>
#include <Ax12.h>
#include <Uart.h>

int main() {
	log << "startup" << endl;

    Board board;
    
	//DmaStream uartDma(1, 4, 7);
	//Configure the pin
	//auto ax12_pin = GpioB[10];
	//Ax12 ax12_broadcast(ax12_pin, Uart(3, &uartDma), 0xfe);
	//Enable torque

	Ax12 ax12_broadcast(board.time, Uart(2), 0xfe, &board.Ax12En, board.Ax12Rx, board.Ax12Tx);
	ax12_broadcast.setSpeed(0x3ff);
	ax12_broadcast.enable();

	while(1) {
		ax12_broadcast.goTo(0x0);
		board.time.msleep(2000);
		ax12_broadcast.goTo(0x280);
		board.time.msleep(2000);
	}
}
