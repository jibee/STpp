#include "Potard.hpp"

Potard::Potard(Platform::Adc& adc, int channel): AdcChannel(adc, channel)
{
    init();
}

Potard::Potard(Platform::Adc& adc, Platform::Gpio& pin): AdcChannel(adc, pin)
{
    init();
}

void Potard::init()
{
    measured_min = 0x0fffffff;
    measured_max = 0;
    effective_min = 0;
    effective_max = 4095;
    using_measured = true;
}



float Potard::getPosition()
{
    unsigned int measure = rawMeasure();
    if(measured_min>measure) measured_min=measure;
    if(measured_max<measure) measured_max=measure;
    if(using_measured)
    {
	if(measured_max>measured_min)
	{
	    return (float)(measure - measured_min) / (float)(measured_max - measured_min);
	}
	else
	{
	    return 0.5;
	}
    }
    else
    {
	return (float)(measure - effective_min) / (float)(effective_max - effective_min);
    }
}

Potard& Potard::setMax(unsigned int value)
{
    if(using_measured)    
	effective_max = value;
    else
	measured_max = value;
    return *this;
}

Potard& Potard::setMin(unsigned int value)
{
    if(using_measured)    
	effective_min = value;
    else
	measured_min = value;
    return *this;
}

Potard& Potard::setAutoLimits(bool active)
{
    using_measured = active;
    return *this;
}

