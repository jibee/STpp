#include <Adc.h>
#include <stm32f4xx.h>
#include <stdlib.h>

using namespace Platform;

Adc::Adc(Adc::AdcConverter converter) {
    switch(converter)
    {
	case ADC_1:
	    {
		m_converter = ADC1;
		RCC->APB2ENR |= 1<<8;
		break;
	    }
	case ADC_2:
	    {
		m_converter = ADC2;
		RCC->APB2ENR |= 1<<9;
		break;
	    }
	case ADC_3:
	    {
		m_converter = ADC3;
		RCC->APB2ENR |= 1<<10;
		break;
	    }
	default:
	    abort();
    }
    //Enable ADC
    m_converter->CR2 |= ADC_CR2_ADON;

    //Disable EOCIE
    m_converter->CR1 &= ~ADC_CR1_EOCIE;

    //Set TSVREFE bit
    ADC->CCR|= ADC_CCR_TSVREFE;

    //Clock =APB1/8
    ADC->CCR |= 2<<16;
}

void Adc::setSamples(int chan, Samples s) {
	int v=0;
	switch(s) {
		case N_480:
			v++;
		case N_144:
			v++;
		case N_112:
			v++;
		case N_84:
			v++;
		case N_56:
			v++;
		case N_28:
			v++;
		case N_15:
			v++;
		case N_3:
			break;
	};
	if(chan>9) {
		chan-=9;
		m_converter->SMPR1= (m_converter->SMPR1 & ~(7<<(3*chan))) | v << (3*chan);
	} else {
		m_converter->SMPR2= (m_converter->SMPR2 & ~(7<<(3*chan))) | v << (3*chan);
}
	}

unsigned int Adc::oneShot(int chan) {
	//Enable ADC clock

	//See procedure p267

	//Only one conversion
	m_converter->SQR1=0;
	m_converter->SQR3=chan;

	//12bits
	m_converter->CR1 &= 3 << 24;

	//Trigger
	m_converter->CR2 |= ADC_CR2_SWSTART;
	
	//Wait for end of conversion
	while( ! (m_converter->SR & ADC_SR_EOC));
	//Read data
	unsigned int val = m_converter->DR&0xfff;
	m_converter->CR2 &= ~ADC_CR2_SWSTART;

	return val;
}

float Adc::getTemperature() {
	setSamples(16, N_480);
	unsigned int ret = oneShot(16);
	
	//Temp = (vsense - v25)/avg_slope + 25
	return ret;
}
