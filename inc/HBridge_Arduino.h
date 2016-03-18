#ifndef _HBRIDGE_ARDUINO_H
#define _HBRIDGE_ARDUINO_H
#include <HBridge.h>
#include <Gpio.h>
#include <Timer.h>
#include <Pwm.h>

#include <Shell.h>
class Shell;

/**
 * Driver for the Arduino Motor Shield (v3) - which happens to be a bizarely
 * connected ST' L298
 *
 * */
template <class TIMER>
class HBridge_Arduino: public HBridge {
    private:
	Platform::Gpio m_dir;
	Platform::Gpio m_brake;
	Pwm<TIMER> m_pwm;
    public:
	HBridge_Arduino(Platform::Gpio& direction, Platform::Gpio& brake, Platform::Gpio& pwm, TIMER& tim, int chan);
	HBridge_Arduino(Platform::Gpio& direction, Platform::Gpio& brake, Pwm<TIMER>& pwm);
	virtual void setSpeed(int);
};

template <class TIMER>
HBridge_Arduino<TIMER>::HBridge_Arduino(Platform::Gpio& d, Platform::Gpio& b, Platform::Gpio& p,
	TIMER& tim, int chan) : m_dir(d), m_brake(b), m_pwm(p, tim, chan, 3, 1024){
    m_dir
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
    m_brake
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
}

template <class TIMER>
HBridge_Arduino<TIMER>::HBridge_Arduino(Platform::Gpio& d, Platform::Gpio& b, Pwm<TIMER>& p): m_dir(d), m_brake(b), m_pwm(p)
{
    m_dir
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
    m_brake
	.setDirection(Platform::Gpio::OUTPUT)
	.setState(false);
}

template <class TIMER>
void HBridge_Arduino<TIMER>::setSpeed(int s){
    if (s==0) {
	m_brake.setState(true);
    } else if (s<0) {
	m_brake.setState(false);
	m_dir.setState(false);
    } else {
	m_brake.setState(false);
	m_dir.setState(true);
    }
    m_pwm.setComparator((s<0) ? -s : s);
}

template<class TIMER_T>
Shell& operator<<(Shell& shell, HBridge_Arduino<TIMER_T>& b)
 {
    if(!shell.got_name) while(1);

    addSetter(shell, b, setSpeed);

    shell.got_name = false;
    return shell;
}


#endif /* _HBRIDGE_ARDUINO_H */
