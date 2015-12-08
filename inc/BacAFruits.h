#ifndef _BACAFRUITS_H
#define _BACAFRUITS_H

#include <Ax12.h>
#include <Time.h>

class BacAFruits {
	private:
		Ax12* external;
		Ax12* centered;
		Ax12* reservoir;
		bool left;
        RTOS::Time& time;
	public:
		BacAFruits(Ax12* external, Ax12* centered, Ax12* reservoir, bool left, RTOS::Time& t);
		BacAFruits& forward();
		BacAFruits& reject(bool leftFruit, bool rightFruit);
		BacAFruits& close();
		BacAFruits& release();
};

#endif /* _BACAFRUITS_H */
