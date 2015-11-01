#ifndef _TIME_H
#define _TIME_H
#include "Timer.h"
namespace Platform
{
class Time {
	private:
		Timer& timer;
	public:
		Time(Timer& timer);
		void usleep(int);
		void msleep(int);
};
}
#endif /* _TIME_H */
