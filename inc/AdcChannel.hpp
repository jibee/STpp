/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Jean-Baptiste Mayer <jibee@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef ADCCHANNEL_H
#define ADCCHANNEL_H

#include <Adc.h>
#include <Gpio.h>

/** Single pin ADC channel.
 * @code{.cpp}
 * Gpio pin;
 * ADC adc;
 * AdcChannel channel = AdcChannel(adc, pin);
 * unsigned int measure = channel.rawMeasure();
 * @endcode
 *
 * The STM32 line of MCU has a limited (2 or so) number of Analog/Digital
 * Converters. However, it is unlikely one would use them simulataneously at
 * their full rate. Therefore, to save device space (and cost) multiple pins
 * can be connected to a single ADC.
 * However obtaining an analog measure on a given pin requires setting up two
 * devices to collaborate (ADC and GPIO) and retaining certain configuration
 * details for each conversion.
 *
 * Instances of this class provide a convenience layer on this model, by
 * linking a GPIO pin and an ADC so that the pin is properly setup and the
 * ugly details of connecting the ADC channel are hidden away from you.
*/
class AdcChannel
{
    public:
	/** Low level constructor
	 *
	 * The use of this constructor is discouraged, and only relevant to monitor the
	 * internal analog channels (temperature, power supply voltage)
	 * It is your responsibility to properly setup the GPIO to ANALOG INPUT
	 * 
	 * @param adc ADC device to employ for conversions
	 * @param adc_channel channel of the ADC to monitor
	 * */
	AdcChannel(Platform::Adc& adc, int adc_channel);
	/** Configures a GPIO as Analog using the given ADC.
	 *
	 * @param adc ADC device to employ for conversions
	 * @param pin GPIO pin to connnect into
	 */
	AdcChannel(Platform::Adc& adc, Platform::Gpio& pin);
	/** Obtains the measure from the relevant ADC channel */
	unsigned int rawMeasure();

    private:
	void init();
	Platform::Adc& adc;
	int channel;
};

#endif // ADCCHANNEL_H
