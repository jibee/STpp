
#include <STM32F4.hpp>

Platform::STM32F4::STM32F4():
    GpioA(GPIOA),
    GpioB(GPIOB),
    GpioC(GPIOC),
    GpioD(GPIOD),
    GpioE(GPIOE),
    GpioF(GPIOF),
    GpioG(GPIOG),
    GpioH(GPIOH),
    GpioI(GPIOI),
    Tim1(TIM1, 1),
    Tim2(TIM2, 2),
    Tim3(TIM3, 3),
    Tim4(TIM4, 4),
    Tim5(TIM5, 5),
    Tim6(TIM6, 6),
    Tim7(TIM7, 7),
    Tim8(TIM8, 8),
    Tim9(TIM9, 9),
    Tim10(TIM10, 10),
    Tim11(TIM11, 11),
    Tim12(TIM12, 12),
    Tim13(TIM13, 13),
    Tim14(TIM14, 14)
{

}

Platform::STM32F4_WithUSB::STM32F4_WithUSB():
STM32F4(),
//USB
USB_Vbus_OC(GpioD[5]),
USB_Vbus_en(GpioC[0]),
USB_Vbus_det(GpioA[9]),
USB_ID(GpioA[10]),
USB_DM(GpioA[11]),
USB_DP(GpioA[12])
{

}
