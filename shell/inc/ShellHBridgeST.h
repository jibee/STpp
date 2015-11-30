#ifndef _SHELL_HBRIDGE_ST_H
#define _SHELL_HBRIDGE_ST_H

#include "Shell.h"
#include "HBridgeST.h"

template<class TIMER_T>
Shell& operator<<(Shell& shell, HBridgeST<TIMER_T>& b)
 {
    if(!shell.got_name) while(1);

    addSetter(shell, b, setSpeed);

    shell.got_name = false;
    return shell;
}

#endif /* _SHELL_HBRIDGE_ST_H */
