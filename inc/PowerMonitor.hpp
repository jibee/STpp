/* ****************************************************************************
 *                                                                            *
 * PowerMonitor.hpp                                                           *
 * Description                                                                *
 *  Input monitor driving power saving functions                              *
 * Function                                                                   *
 *                                                                            *
 *****************************************************************************/
/* ****************************************************************************
 *                                                                            *
 * Created on: 2017-01-25T14:47:06                                            *
 * 4.4.0-59-generic #80-Ubuntu SMP Fri Jan 6 17:47:47 UTC 2017                *
 * Revisions:                                                                 *
 *                                                                            *
 *****************************************************************************/
/******************************************************************************
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 ******************************************************************************

 ******************************************************************************
 *  Contributors:                                                             *
 * (c) 2007 Jean-Baptiste Mayer (jibee@jibee.com) (initial work)              *
 *  Name/Pseudo <email>                                                       *
 ******************************************************************************/

#ifndef POWERMONITOR
#define POWERMONITOR

class IPowerSavingDevice;

class PowerMonitor
{
    public:
	PowerMonitor();
	void updatePowerStatus(IPowerSavingDevice* device);
	void reportInteraction();
    private:
	unsigned int m_idleCycles;
	bool m_isPowerSaving;
	const static unsigned int IDLE_THRESHOLD;
	const static unsigned int SLEEP_THRESHOLD;
};

#endif /* POWERMONITOR */

