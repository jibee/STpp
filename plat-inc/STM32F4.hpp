/**
 * 
 * */

#ifndef PLAT_SMT32F4_H
#define PLAT_SMT32F4_H

#include "Gpio.h"
#include "Timer.h"

namespace Platform
{
class STM32F4
{
public:
    STM32F4();
        
        /** @name Static GPIO ports definitions.
    *
    * */
    ///@{
    /** GPIO Port A */
    GpioPort GpioA;
    /** GPIO Port B */
    GpioPort GpioB;
    /** GPIO Port B */
    GpioPort GpioC;
    /** GPIO Port D */
    GpioPort GpioD;
    /** GPIO Port E */
    GpioPort GpioE;
    /** GPIO Port F */
    GpioPort GpioF;
    /** GPIO Port G */
    GpioPort GpioG;
    /** GPIO Port H */
    GpioPort GpioH;
    /** GPIO Port I */
    GpioPort GpioI;
    ///@}

        
    /** @name Timers
    *
    * */
    ///@{
    /** Timer1 */
    AdvancedControlTimer Tim1;
    /** Timer2 */
    GeneralPurposeTimer<uint32_t,4> Tim2;
    /** Timer3 */
    GeneralPurposeTimer<uint16_t,4> Tim3;
    /** Timer4 */
    GeneralPurposeTimer<uint16_t,4> Tim4;
    /** Timer5 */
    GeneralPurposeTimer<uint32_t,4> Tim5;
    /** Timer6 */
    Timer Tim6;
    /** Timer7 */
    Timer Tim7;
    /** Timer8 */
    AdvancedControlTimer Tim8;
    /** Timer9 */
    GeneralPurposeTimer<uint16_t,2> Tim9;
    /** Timer10 */
    GeneralPurposeTimer<uint16_t,1> Tim10;
    /** Timer11 */
    GeneralPurposeTimer<uint16_t,1> Tim11;
    /** Timer12 */
    GeneralPurposeTimer<uint16_t,2> Tim12;
    /** Timer13 */
    GeneralPurposeTimer<uint16_t,1> Tim13;
    /** Timer14 */
    GeneralPurposeTimer<uint16_t,1> Tim14;
    ///@}

};

class STM32F4_WithUSB: public STM32F4
{
    public:
    STM32F4_WithUSB();
    //USB
    Gpio USB_Vbus_OC;
    Gpio USB_Vbus_en;
    Gpio USB_Vbus_det;
    Gpio USB_ID;
    Gpio USB_DM;
    Gpio USB_DP;
};
}

#endif /* PLAT_SMT32F4_H */