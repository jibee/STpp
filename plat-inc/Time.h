#ifndef _TIME_H
#define _TIME_H
#include "Timer.h"
namespace RTOS
{
    /** Sleep operations
     * */
class Time {
	private:
		Platform::Timer& timer;
	public:
        /** Constructor
         * 
         * @param timer hardware timer to use for microsleeps.
         */
		Time(Platform::Timer& timer);
        /** Sleeps a certain number of microseconds
         * 
         * @param useconds time to sleep for in microseconds
         * @todo check the exact unit. The code indicates it is actually 
         * the number of ticks in the timer.
         */
		void usleep(int useconds);
        /** Sleeps a certain number of milliseconds
         * @param mseconds time to sleep in milliseconds
         * @warning this does not use hardware timers
         */
		void msleep(int mseconds);
};
}
#endif /* _TIME_H */
