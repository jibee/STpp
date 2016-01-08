#include <CherryPickerBoard.h>
#include "HBridgeST.h"

int main() {
	CherryPickerBoard b;
	HBridgeST<GeneralPurposeTimer<uint32_t, 4>> motor0(b.Prop0A, b.Prop0B, b.Prop0_PWM, b.Tim2, 4);

	int v(0);

	while(1) {
		v+=100;
		v%= 1024;
		motor0.setSpeed(v);
		b.time.msleep(500);
	}
}
