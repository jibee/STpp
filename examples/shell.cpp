#include <CherryPickerBoard.h>
#include <tr1/functional>
#include <Log.h>
#include <Debug.h>
#include <Exti.h>
#include <UsbSerial.h>
#include <Shell.h>
#include <ShellAsserv.h>
#include <ShellAx12.h>
#include <ShellBacAFruits.h>
#include <ShellGpio.h>
#include <ShellHBridgeST.h>
#include <ShellIncrementalEncoder.h>
#include <ShellPwm.h>
#include <ShellStrategie.h>
#include <ShellTimer.h>
#include <Lidar.h>
#include <Lidar.h>
#include <Tasks.h>
#include <Gpio.h>

static Shell shell;

int main() {
    CherryPickerBoard b;
	UsbSerial usb(b);

	RTOS::Task lidar_task([&usb, &b]() {
		LidarNeato lidar(b.External_RX, b.ExternalUart);
		while(true) {
			lidar_neato_t lidar_packet;
			lidar >> lidar_packet;
		}
	}, "Lidar reader");

	usb << "Hello !" << endl;
	shell.setStream(&usb, &usb);

	Asserv<GeneralPurposeTimer<uint32_t,4>> asserv(b.Encoder1, b.Encoder0, b.Tim13, b.HBridge1, b.HBridge0);
	//Use _ prefix as a hideme for autocompletion
	shell << "_UserButton" << b.UserButton;

	shell << "LedG" << b.LedG;
	shell << "LedO" << b.LedO;
	shell << "LedR" << b.LedR;
	shell << "LedB" << b.LedB;

	shell << "_Prop0A" << b.Prop0A;
	shell << "_Prop0B" << b.Prop0B;

	shell << "_Prop1A" << b.Prop1A;
	shell << "_Prop1B" << b.Prop1B;

	shell << "HBridge0" << b.HBridge0;
	shell << "HBridge1" << b.HBridge1;

	shell << "Asserv" << asserv;

	shell << "Encoder0" << b.Encoder0;
	shell << "Encoder1" << b.Encoder1;

	Strategie strategie(b.mamoutorRight, asserv, b.time);
	shell << "Strategie" << strategie;

	shell << "Mamoutor" << b.mamoutorRight;

	shell << "bacLeftExternal" << b.bacLeftExternal;
	shell << "bacLeftCentered" << b.bacLeftCentered;
	shell << "bacLeftReservoir" << b.bacLeftReservoir;

	shell << "bacRightCentered" << b.bacRightCentered;
	shell << "bacRightReservoir" << b.bacRightReservoir;

	shell << "bacRight" << b.bacRight;
	shell << "bacLeft" << b.bacLeft;

	shell << "Ax12" << b.ax12Broadcast;

	shell.add([&asserv, &b/*,&mamoutor*/](Stack& s) {
		(void)s;
		asserv.reset();
		//mamoutor.disable();
		b.Encoder0.setValue(0);
		b.Encoder1.setValue(0);
	}, "reset");

	shell.add([&usb](Stack& s) {
		(void)s;
		int angle = s.pop().toInt();
		usb << "Lidar distance " << LidarNeato::getDistance(angle) << endl;
	}, "lidar");

	shell.add([&usb](Stack& s) {
		(void)s;
		for(int angle=0; angle<360; ++angle)
			usb << LidarNeato::getDistance(angle) << ", ";
		usb << endl;
	}, "lidar", "dump");

	shell.add([&usb,&asserv](Stack& s) {
		(void)s;
		usb << asserv.getPosition();
	}, "whereami");

	shell.exec();
	while(1);
}
