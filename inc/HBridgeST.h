#ifndef _HBRIDGE_ST_H
#define _HBRIDGE_ST_H
#include <Gpio.h>
#include <Timer.h>
#include <Pwm.h>

#include <Shell.h>
class Shell;

/**
 * Driver for a directly-connected integrated H-bridge such as ST' L298
 *
 * */
template <class TIMER>
class HBridgeST {
    private:
	Platform::Gpio propa;
	Platform::Gpio propb;
	Pwm<TIMER> pwm;
    public:
	HBridgeST(Platform::Gpio& a, Platform::Gpio& b, Platform::Gpio& pwm, TIMER& tim, int chan);
	void setSpeed(int);
};

template <class TIMER>
HBridgeST<TIMER>::HBridgeST(Platform::Gpio& a, Platform::Gpio& b, Platform::Gpio& p,
	TIMER& tim, int chan) : propa(a), propb(b), pwm(p, tim, chan, 3, 1024){
    propa
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
    propb
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
}

template <class TIMER>
void HBridgeST<TIMER>::setSpeed(int s){
    if (s==0) {
	propa.setState(false);
	propb.setState(false);
    } else if (s<0) {
	propa.setState(true);
	propb.setState(false);
    } else {
	propa.setState(false);
	propb.setState(true);
    }
    pwm.setComparator((s<0) ? -s : s);
}

template<class TIMER_T>
Shell& operator<<(Shell& shell, HBridgeST<TIMER_T>& b)
 {
    if(!shell.got_name) while(1);

    addSetter(shell, b, setSpeed);

    shell.got_name = false;
    return shell;
}


#endif /* _HBRIDGE_ST_H */
