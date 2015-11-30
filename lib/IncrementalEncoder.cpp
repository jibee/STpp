#include <IncrementalEncoder.h>

using namespace Platform;

IncrementalEncoder::IncrementalEncoder(Gpio& a, Gpio& b, bool reverse):
encoderA(a), encoderB(b), reverse(reverse)
{
    a
        .setSpeed(Gpio::SPEED_100MHz)
        .setDirection(Gpio::INPUT)
        .setResistor(Gpio::PULL_UP);
    b

        .setSpeed(Gpio::SPEED_100MHz)
        .setDirection(Gpio::INPUT)
        .setResistor(Gpio::PULL_UP);

    counter = 0;
    last_value = 0;
}




/**
 * @brief Sets the counter value to the argument value
 *
 */
int IncrementalEncoder::operator=(int v) {
    setValue(v);
    return v;
}

void IncrementalEncoder::setValue(int v)
{
	counter = reverse ? -v : v;
}





/**
 * @brief Gives out the current absolute value of the counter
 *
 */
int IncrementalEncoder::getCounter(){
	update();
	if(reverse)
		return -counter;
	return counter;
}

/**
 * @brief Gets the value from the getCounter function call 
 *
 */
IncrementalEncoder::operator int() {
	return getCounter();
}

