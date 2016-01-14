#include <DiscoveryBoard.hpp>
#include "HBridgeST.h"
#include <Time.h>

int main() {
    DiscoveryBoard b;
    Gpio Prop0_PWM(b.GpioB[10]);
    Gpio Prop0A(b.GpioB[13]);
    Gpio Prop0B(b.GpioB[15]);
    RTOS::Time time(b.Tim7);
    HBridgeST<GeneralPurposeTimer<uint32_t, 4>> motor0(Prop0A, Prop0B, Prop0_PWM, b.Tim2, 4);

    int v(0);

    while(1) {
	v+=100;
	v%= 1024;
	motor0.setSpeed(v);
	time.msleep(500);
    }
}
