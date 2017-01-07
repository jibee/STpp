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

#ifndef ANALOGDEBOUNCER_H
#define ANALOGDEBOUNCER_H

int floor(float val);

template<int TABS, int SAMPLES>
class AnalogDebouncer
{
public:
    AnalogDebouncer()
    {
        dataPointer = 0;
        fill(0.5);
    }
    void fill(float f)
    {
	ewma = f;
        for(int i = 0; i<SAMPLES; ++i)
        {
            putValue(f);
        }
	lastIn = (float)TABS * average();
	lastOut = floor(lastIn);
    }
    int process(float f)
    {
	putValue(f);
	float d = (float)TABS * average();
	float diff = d - lastIn;
	if(diff> 0.25 || diff < -0.25)
	{
	    int retval = floor(d);
	    retval = retval<TABS?(retval<0?0:retval):(TABS-1);
	    if(retval!=lastOut)
	    {
		lastOut = retval;
		lastIn = d;
	    }
	}
	return lastOut;
    }
private:
    float history[SAMPLES];
    float lastIn;
    int lastOut;
    int dataPointer;
    float ewma;
    void putValue(float f)
    {
	dataPointer++;
	if(TABS == dataPointer)
	{
	    dataPointer = 0;
	}
	history[dataPointer]=f;
	ewma = (ewma * 0.9 + f) / 1.9;
    }
    float average()
    {
	return ewma;
    }
};

#endif // ANALOGDEBOUNCER_H
