#ifndef ANATOLEBOX_POTARD_HPP
#define ANATOLEBOX_POTARD_HPP

#include <AdcChannel.hpp>


class Potard: public AdcChannel
{
public:
    Potard(Platform::Adc& adc, int adc_channel);
    Potard(Platform::Adc& adc, Platform::Gpio& pin);
    float getPosition();
    Potard& setMax(unsigned int value);
    Potard& setMin(unsigned int value);
    Potard& setAutoLimits(bool active=true);
private:
    void init();
    unsigned int measured_max;
    unsigned int measured_min;
    unsigned int effective_max;
    unsigned int effective_min;
    bool using_measured; 
};

#endif /* ANATOLEBOX_POTARD_HPP */
