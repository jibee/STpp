/* ****************************************************************************
 *                                                                            *
 * IPowerSavingDevice.hpp                                                     *
 * Description                                                                *
 *                                                                            *
 * Function                                                                   *
 *                                                                            *
 *****************************************************************************/
/* ****************************************************************************
 *                                                                            *
 * Created on: 2017-01-23T18:02:07                                            *
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

#ifndef IPOWERSAVINGDEVICE
#define IPOWERSAVINGDEVICE
/** Interface to be implemented by power hungry devices so they can be
  * gracefully switched off when the device is idle
  */

class IPowerSavingDevice
{
    public:
	virtual ~IPowerSavingDevice();
	/** Enter early power saving measures, ie reduce power consumption assuming the user is away */
	virtual void enterIdleMode();
	/** Enter stronger power saving measures, ie shutdown most power consuming devices */
	virtual void enterSleepMode();
	/** Enter full activity mode */
	virtual void enterActiveMode();
};

#endif /* IPOWERSAVINGDEVICE */

