#ifndef _WATCHDOG_H
#define _WATCHDOG_H
namespace Platform
{

/** Clock-independant watchdog.

This class controls the watchdog hardware. This is essentially a time counter
which resets the CPU if not heartbeat is recorded for a certain amount of time.

*/
class Watchdog {
    public:
	/**
	  Setup the watchdog timing.

	  The watchdog operates at a separate clock speed which is typically 32kHz but
	  can vary from 30kHz to 60kHz.
	  The wathdog will reset the CPU after a certain number of ticks, which is
	  calculated by mutliplying the prescaler with the autoreload
	 */
	Watchdog(int prescaler, int autoreload);
	/** Starts the watchdog. From now on the software must call @link reset @endlink
	  regularly to prevent the watchdog to automatically reset the CPU.
	 */
	Watchdog& start();
	/** Tells the watchdog that the software is alive. */
	Watchdog& reset();
	/** Suspend the watchdog when the dubugger stops the CPU */
	Watchdog& stopOnDebug();
	/** Do not suspend the watchdog when the dubugger stops the CPU */
	Watchdog& continueOnDebug();
};

}
#endif /* _WATCHDOG_H */
