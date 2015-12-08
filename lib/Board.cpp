#include "Board.h"
#include "Led.h"
#include "Pwm.h"
#include "HBridgeST.h"
#include "IncrementalEncoder.h"
#include "Uart.h"
#include "Ax12.h"
using namespace Platform;

Board::Board(): DiscoveryBoard(),
ZigbeeUartDma(DmaStream::DMAController1, DmaStream::S4, DmaStream::C4),
g_RLed1(GpioD[0]),
g_RLed2(GpioD[1]),
g_RLed3(GpioD[2]),
g_RLed4(GpioD[3]),
Ax12Tx(GpioA[2]),
Ax12En(GpioA[1]),
Ax12Rx(GpioA[3]),
Ax12UartDma(DmaStream::DMAController1, DmaStream::S6, DmaStream::C4),
ExternalUartDma(DmaStream::DMAController1, DmaStream::S3, DmaStream::C4),

// Motor control
Prop0_PWM(GpioB[10]),
Prop0A(GpioB[13]),
Prop0B(GpioB[15]),
HBridge0(Prop0A, Prop0B, Prop0_PWM, Tim2, 3),

Enc0A(GpioE[9]),
Enc0B(GpioE[11]),
Encoder0(Enc0A, Enc0B, Tim1, 0, false),

Prop1_PWM(GpioB[11]),
Prop1A(GpioB[14]),
Prop1B(GpioB[12]),
HBridge1(Prop1B, Prop1A, Prop1_PWM, Tim2, 4),

Enc1A(GpioB[4]),
Enc1B(GpioB[5]),
Encoder1(Enc1A, Enc1B, Tim3, 0, true),

Prop2_PWM(GpioC[7]),
Prop2A(GpioC[8]),
Prop2B(GpioC[12]),
Hbridge2(Prop2A, Prop2B, Prop2_PWM, Tim8, 2),

Prop3_PWM(GpioC[6]),
Prop3A(GpioC[9]),
Prop3B(GpioC[13]),
Hbridge3(Prop3A, Prop3B, Prop3_PWM, Tim8, 1),


//Zigbee
Zigbee_RX(GpioC[11]),
Zigbee_TX(GpioC[10]),
//TODO: Need a Zigbee class
Zigbee_UART(4, &ZigbeeUartDma),

//On-board leds
/*
 *      O
 *  G        R
 *      G
 */
LedG(g_LedG, Tim4, 1),
LedO(g_LedO, Tim4, 2),
LedR(g_LedR, Tim4, 3),
LedB(g_LedB, Tim4, 4),

//Other LEDs
//No pwm available for those

//Extra
UserButton(GpioA[0]),

//Which timer to use for precise (<1ms) timing
time(Tim14),

Ax12_UART(2, &Ax12UartDma),

mamoutorRight(time, Ax12_UART, 144, &Ax12En, Ax12Rx, Ax12Tx),

bacLeftExternal(time, Ax12_UART, 141, &Ax12En, Ax12Rx, Ax12Tx),
bacLeftCentered(time, Ax12_UART, 129, &Ax12En, Ax12Rx, Ax12Tx),
bacLeftReservoir(time, Ax12_UART, 121, &Ax12En, Ax12Rx, Ax12Tx),

bacRightCentered(time, Ax12_UART, 20, &Ax12En, Ax12Rx, Ax12Tx),
bacRightReservoir(time, Ax12_UART, 142, &Ax12En, Ax12Rx, Ax12Tx),

ax12Broadcast(time, Ax12_UART, 0xfe, &Ax12En, Ax12Rx, Ax12Tx),

bacLeft(&bacLeftExternal, &bacLeftCentered, &bacLeftReservoir, true, time),
bacRight(NULL, &bacRightCentered, &bacRightReservoir, false, time),

External_TX(GpioD[8]),
External_RX(GpioD[9]),
ExternalUart(3, &ExternalUartDma)
{

}
