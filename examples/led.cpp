#include <DiscoveryBoard.hpp>
#include <Log.h>
#include <Time.h>
#include <Pwm.h>
DiscoveryBoard board;

int main() {
    Log::log << "startup" << endl;

    RTOS::Time time(board.Tim7);

    board.g_LedB.setFunction(Gpio::GPIO).setDirection(Gpio::OUTPUT).setPushPull().setSpeed(Gpio::SPEED_2MHz).setState(true);
    board.g_LedG.setFunction(Gpio::GPIO).setDirection(Gpio::OUTPUT).setPushPull().setSpeed(Gpio::SPEED_100MHz).setState(true);
    board.g_LedO.setFunction(Gpio::GPIO).setDirection(Gpio::OUTPUT).setPushPull().setSpeed(Gpio::SPEED_50MHz).setState(true);
    board.g_LedR.setFunction(Gpio::GPIO).setDirection(Gpio::OUTPUT).setPushPull().setSpeed(Gpio::SPEED_25MHz).setState(true);
    time.msleep(500);

    
    Log::log<<"started"<<endl;
    for(int i=0; i<5; ++i)
    {
        for(int j=0; j<4; ++j)
        {
            board.g_LedB.setState(0==j);
            board.g_LedG.setState(1==j);
            board.g_LedO.setState(2==j);
            board.g_LedR.setState(3==j);
            time.msleep(1000);
        }
    }
    Log::log<<"Finished :("<<endl;
    
//On-board leds
/*
 *      O
 *  G        R
 *      G
 */
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedG(board.g_LedG, board.Tim4, 1);
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedO(board.g_LedO, board.Tim4, 2);
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedR(board.g_LedR, board.Tim4, 3);
    Pwm<GeneralPurposeTimer<uint16_t,4>> LedB(board.g_LedB, board.Tim4, 4);

    int r = 0, b = 2, g = 4, o = 6;
    Log::log<<"started"<<endl;
	while(1) {
		++r;
		++g;
		++b;
		++o;
		r%=10;
		g%=10;
		b%=10;
		o%=10;

		LedO.setComparator(1<<o);
		LedG.setComparator(1<<g);
		LedB.setComparator(1<<b);
		LedR.setComparator(1<<r);
		time.msleep(200);
	}
}
