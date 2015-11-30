#include <Led.h>
#include <Pwm.h>
#include <Time.h>
#include <BacAFruits.h>
#include <HBridgeST.h>
#include <IncrementalEncoder.h>
#include <Uart.h>
#include <Ax12.h>

using namespace Platform;

// Motor control
extern Gpio Prop0_PWM;
extern Gpio Prop0A;
extern Gpio Prop0B;
extern HBridgeST<GeneralPurposeTimer<uint32_t,4> > HBridge0;

extern Gpio Enc0A;
extern Gpio Enc0B;
extern IncrementalEncoder16 Encoder0;

extern Gpio Prop1_PWM;
extern Gpio Prop1A;
extern Gpio Prop1B;
extern HBridgeST<GeneralPurposeTimer<uint32_t,4>> HBridge1;

extern Gpio Enc1A;
extern Gpio Enc1B;
extern IncrementalEncoder16 Encoder1;

extern Gpio Prop2_PWM;
extern Gpio Prop2A;
extern Gpio Prop2B;
extern HBridgeST<AdvancedControlTimer> Hbridge2;

extern Gpio Prop3_PWM;
extern Gpio Prop3A;
extern Gpio Prop3B;
extern HBridgeST<AdvancedControlTimer> Hbridge3;

//USB
extern Gpio USB_Vbus_OC;
extern Gpio USB_Vbus_en;
extern Gpio USB_Vbus_det;
extern Gpio USB_ID;
extern Gpio USB_DM;
extern Gpio USB_DP;

//On-board leds
extern Pwm<GeneralPurposeTimer<uint16_t,4>> LedG;
extern Pwm<GeneralPurposeTimer<uint16_t,4>> LedO;
extern Pwm<GeneralPurposeTimer<uint16_t,4>> LedB;
extern Pwm<GeneralPurposeTimer<uint16_t,4>> LedR;

//Extra
extern Gpio UserButton;

//Which timer to use for precise (<1ms) timing
extern RTOS::Time time;

extern Gpio Zigbee_RX;
extern Gpio Zigbee_TX;
extern Uart Zigbee_UART;

extern Gpio Ax12TX;
extern Gpio Ax12RX;
extern Gpio Ax12EN;
extern Uart Ax12_UART;

extern Ax12 mamoutorRight;
extern Ax12 bacLeftExternal;
extern Ax12 bacLeftCentered;
extern Ax12 bacLeftReservoir;
extern Ax12 bacRightCentered;
extern Ax12 bacRightReservoir;
extern Ax12 ax12Broadcast;

extern BacAFruits bacLeft;
extern BacAFruits bacRight;

extern Gpio External_TX;
extern Gpio External_RX;
extern Uart ExternalUart;
