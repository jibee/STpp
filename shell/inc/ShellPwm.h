#ifndef _SHELL_PWM_H
#define _SHELL_PWM_H

#include "Shell.h"
#include "Pwm.h"

template<class TIMER_T>
Shell& operator<<(Shell& shell, Pwm<TIMER_T>& pwm)
{
    if(!shell.got_name) while(1);

    addSetter(shell, pwm, setComparator);
    addSetter(shell, pwm, setDutyCycle);

    shell.got_name = false;
    return shell;
}

#endif /* _SHELL_PWM_H */
