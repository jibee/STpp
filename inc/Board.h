#ifndef PLATFORM_BOARD_H
#define PLATFORM_BOARD_H

#include <Led.h>
#include <Pwm.h>
#include <Time.h>
#include <BacAFruits.h>
#include <HBridgeST.h>
#include <IncrementalEncoder.h>
#include <Uart.h>
#include <Ax12.h>
#include <DiscoveryBoard.hpp>


using namespace Platform;

/** 
 * Definition of the peripherals for a "cherry picker" robot.
 * 
 * This is specific to a robotics contest held in 20??
 * 
 * @todo rename to CherryPickerBoard ?
 * 
 * */
class Board: public DiscoveryBoard
{
    private:
	DmaStream ZigbeeUartDma;
    public:
	Gpio g_RLed1;
	Gpio g_RLed2;
	Gpio g_RLed3;
	Gpio g_RLed4;
	Gpio Ax12Tx;
	Gpio Ax12En;
	Gpio Ax12Rx;
    private:
	DmaStream Ax12UartDma;
	DmaStream ExternalUartDma;
    public:
	Board();

	// Motor control
	Gpio Prop0_PWM;
	Gpio Prop0A;
	Gpio Prop0B;
	HBridgeST<GeneralPurposeTimer<uint32_t,4> > HBridge0;

	Gpio Enc0A;
	Gpio Enc0B;
	IncrementalEncoder16 Encoder0;

	Gpio Prop1_PWM;
	Gpio Prop1A;
	Gpio Prop1B;
	HBridgeST<GeneralPurposeTimer<uint32_t,4>> HBridge1;

	Gpio Enc1A;
	Gpio Enc1B;
	IncrementalEncoder16 Encoder1;

	Gpio Prop2_PWM;
	Gpio Prop2A;
	Gpio Prop2B;
	HBridgeST<AdvancedControlTimer> Hbridge2;

	Gpio Prop3_PWM;
	Gpio Prop3A;
	Gpio Prop3B;
	HBridgeST<AdvancedControlTimer> Hbridge3;

	Gpio Zigbee_RX;
	Gpio Zigbee_TX;
	Uart Zigbee_UART;


	//On-board leds
	Pwm<GeneralPurposeTimer<uint16_t,4>> LedG;
	Pwm<GeneralPurposeTimer<uint16_t,4>> LedO;
	Pwm<GeneralPurposeTimer<uint16_t,4>> LedR;
	Pwm<GeneralPurposeTimer<uint16_t,4>> LedB;

	//Extra
	Gpio UserButton;

	//Which timer to use for precise (<1ms) timing
	RTOS::Time time;

	Uart Ax12_UART;

	Ax12 mamoutorRight;
	Ax12 bacLeftExternal;
	Ax12 bacLeftCentered;
	Ax12 bacLeftReservoir;
	Ax12 bacRightCentered;
	Ax12 bacRightReservoir;
	Ax12 ax12Broadcast;

	BacAFruits bacLeft;
	BacAFruits bacRight;

	Gpio External_TX;
	Gpio External_RX;
	Uart ExternalUart;
};

#endif /* PLATFORM_BOARD_H */

