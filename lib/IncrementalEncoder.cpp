#include <IncrementalEncoder.h>
#include <Shell.h>

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


Shell& operator<<(Shell& shell, IncrementalEncoder& enc) {
    if(!shell.got_name) while(1);

    shell.add([&enc](Stack& s) {
        s.push(enc.getCounter());
    }, shell.current_object);

    shell.got_name = false;
    return shell;
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

