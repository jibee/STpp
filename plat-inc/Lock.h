#ifndef LOCK_H
#define LOCK_H

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
};
namespace Platform
{
/** Mutex (implemented with a semaphore) */
class Mutex {
    private:
	xSemaphoreHandle sem;
    public:
	/** Constructor */
	Mutex();
	/** Acquire the lock */
	void lock();
	/** Release the lock */
	void unlock();
};
/** Binary semaphore - used for pure synchronisation between tasks */
class BinarySemaphore {
    private:
	xSemaphoreHandle sem;
    public:
/** Constructor */
	BinarySemaphore();
/** Takes the semaphore */
	void take();
/** Gives the semaphore */
	void give();
/** Tries to take the semaphore
@return true if the semaphore was successfully taken */
	bool tryTake();
};
/** Resource locker - automatically releases the mutex when goes out of scope. */
class AutoLock {
    private:
	Mutex _m;
    public:
	/** Constructor
	  @param m Mutex to use for locking
	 */
	AutoLock(Mutex m);
	~AutoLock();
};
}

#endif /* LOCK_H */
