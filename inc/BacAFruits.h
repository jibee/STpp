#ifndef _BACAFRUITS_H
#define _BACAFRUITS_H

#include <Ax12.h>
#include <Time.h>
/**
 * Composite device comprising three servo motors (AX12).
 *
 * @TODO move to examples as this is really specific to one application
 */
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

class Shell;

Shell& operator<<(Shell&,BacAFruits&);

#endif /* _BACAFRUITS_H */
