#ifndef _USB_H
#define _USB_H

#include "STM32F4.hpp"

namespace Platform
{
class Usb {
	public:
		Usb(STM32F4_WithUSB& board);
};
}
#endif /* _USB_H */
