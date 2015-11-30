#include "Timer.h"

namespace Platform
{

    AdvancedControlTimer Tim1(TIM1, 1);
    GeneralPurposeTimer<uint32_t,4> Tim2(TIM2, 2);
    GeneralPurposeTimer<uint16_t,4> Tim3(TIM3, 3);
    GeneralPurposeTimer<uint16_t,4> Tim4(TIM4, 4);
    GeneralPurposeTimer<uint32_t,4> Tim5(TIM5, 5);
    Timer Tim6(TIM6, 6);
    Timer Tim7(TIM7, 7);
    AdvancedControlTimer Tim8(TIM8, 8);
    GeneralPurposeTimer<uint16_t,2> Tim9(TIM9, 9);
    GeneralPurposeTimer<uint16_t,1> Tim10(TIM10, 10);
    GeneralPurposeTimer<uint16_t,1> Tim11(TIM11, 11);
    GeneralPurposeTimer<uint16_t,2> Tim12(TIM12, 12);
    GeneralPurposeTimer<uint16_t,1> Tim13(TIM13, 13);
    GeneralPurposeTimer<uint16_t,1> Tim14(TIM14, 14);
}
