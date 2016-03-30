#ifndef _PWM_H
#define _PWM_H
#include <Gpio.h>
#include <Timer.h>
#include <Shell.h>

template <class TIMER>
class Pwm {
    private:
	Platform::Gpio& gpio;
	TIMER& tim;
	int m_channel;
    public:
	Pwm(Platform::Gpio& p, TIMER& t, int channel, int pre=42, int atr=1024);
	Pwm& setDutyCycle(float v);
	Pwm& setDutyCycle(int percent);
	Pwm& setComparator(unsigned int cmp);
};


template <class TIMER>
Pwm<TIMER>::Pwm(Platform::Gpio& p, TIMER& t, int channel, int pre, int atr) :
    gpio(p), tim(t), m_channel(channel) {

    tim.setAlternate(gpio);

    gpio
        .setSpeed(Platform::Gpio::SPEED_100MHz)
        .setFunction(Platform::Gpio::ALTERNATE)
        .setDirection(Platform::Gpio::OUTPUT)
        .setResistor(Platform::Gpio::NONE)
        .setPushPull();
    tim
        .setPrescaler(pre)
        .setAutoReload(atr)
        .setOneShot(false);
    tim
        .setChannelComparator(m_channel, 0)
        .setChannelDirection(m_channel, Platform::Timer::OUTPUT)
        .setChannelMode(m_channel, Platform::Timer::CompareHigh)
        .setChannelOutput(m_channel, true)
        .enable();
}

template <class TIMER>
Pwm<TIMER>& Pwm<TIMER>::setDutyCycle(float v) {
    tim.setChannelComparator(m_channel, v*tim.getAutoReload());
    return *this;
}

template <class TIMER>
Pwm<TIMER>& Pwm<TIMER>::setDutyCycle(int percent) {
    tim.setChannelComparator(m_channel, (int)(((float)percent/100.0)*((float)tim.getAutoReload())));
    return *this;
}

template <class TIMER>
Pwm<TIMER>& Pwm<TIMER>::setComparator(unsigned int cmp) {
    tim.setChannelComparator(m_channel, cmp);
    return *this;
}

template<class TIMER_T>
Shell& operator<<(Shell& shell, Pwm<TIMER_T>& pwm)
{
    if(!shell.got_name) while(1);

    addSetter(shell, pwm, setComparator);
    addSetter(shell, pwm, setDutyCycle);

    shell.got_name = false;
    return shell;
}

#endif /* _PWM_H */
