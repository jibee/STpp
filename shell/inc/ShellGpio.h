#ifndef _SHELL_GPIO_H
#define _SHELL_GPIO_H

#include "Shell.h"
#include "Gpio.h"

Shell& operator<<(Shell&, Platform::Gpio&);

#endif /* _SHELL_H */