#ifndef ANATOLEBOX_POTARD_HPP
#define ANATOLEBOX_POTARD_HPP

#include <Adc.h>
#include <Gpio.h>

class Potard
{
public:
    Potard(Platform::Adc& adc, int adc_channel);
    Potard(Platform::Adc& adc, Platform::Gpio& pin);
    unsigned int rawMeasure();
    float getPosition();
    Potard& setMax(unsigned int value);
    Potard& setMin(unsigned int value);
    Potard& setAutoLimits(bool active=true);
private:
    Platform::Adc& adc;
    int channel;
    void init();
    unsigned int measured_max;
    unsigned int measured_min;
    unsigned int effective_max;
    unsigned int effective_min;
    bool using_measured; 
};

#endif /* ANATOLEBOX_POTARD_HPP */
