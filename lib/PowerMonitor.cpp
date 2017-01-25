/* ****************************************************************************
 *                                                                            *
 * PowerMonitor.cpp                                                           *
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

#include "PowerMonitor.hpp"
#include "IPowerSavingDevice.hpp"

const unsigned int PowerMonitor::IDLE_THRESHOLD = 400;
const unsigned int PowerMonitor::SLEEP_THRESHOLD = PowerMonitor::IDLE_THRESHOLD * 10;

PowerMonitor::PowerMonitor(): m_idleCycles(0), m_isPowerSaving(false)
{
}

void PowerMonitor::updatePowerStatus(IPowerSavingDevice* device)
{
    if(IDLE_THRESHOLD==m_idleCycles)
    {
	m_isPowerSaving = true;
	device->enterIdleMode();
    }
    if(SLEEP_THRESHOLD==m_idleCycles)
    {
	m_isPowerSaving = true;
	device->enterSleepMode();
    }
    if(0==m_idleCycles && m_isPowerSaving)
    {
	m_isPowerSaving = false;
	device->enterActiveMode();
    }
    m_idleCycles++;
}

void PowerMonitor::reportInteraction()
{
    m_idleCycles=0;
}

