#ifndef _ADC_H
#define _ADC_H


#include <stm32f4xx.h>
namespace Platform
{
/**
Analog to digital converter

*/
class Adc {
	public:
	    enum AdcConverter
	    {
		ADC_1,
		ADC_2,
		ADC_3
	    };
/** Constructor */
		Adc(Adc::AdcConverter adc);
/** Obtains a single reading of the analog value for a given channel

Note this function will block during the conversion.

@param chan ADC channel number. This is a number from 0 to 18, where the latter
three channels (16, 17 and 18) reflect internal temperature sensors and battery
tension respectively.

@return the ADC measurement

TODO add explicit connection to GPIO 
TODO can we set the number of bits of precision required?
TODO add a non-blocking version?
*/
		unsigned int oneShot(int chan);
/** Number of ADC clock cycles during which the input is sampled
*/
		enum Samples {
			N_3,
			N_15,
			N_28,
			N_56,
			N_84,
			N_112,
			N_144,
			N_480,
		};
/** Sets the number of ADC clock cycles during which the analog input is
averaged.

@param chan ADC channel number. This is a number from 0 to 18, where the latter
three channels (16, 17 and 18) reflect internal temperature sensors and battery
tension respectively.
@param s number of ADC Clock cycles during which the input is sampled
*/
		void setSamples(int chan, Samples s);
/** Obtains the chip temperature. */
		float getTemperature();
	private:
		ADC_TypeDef* m_converter;
};
}
#endif /* _ADC_H */
