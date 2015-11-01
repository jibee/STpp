#include <stm32f4xx.h>
#include "Irq.h"

using namespace Platform;

Irq::Irq(int n) : irq(n) {
}

Irq& Irq::enable() {
	NVIC->ISER[ irq >> 5] |= 1<<(irq & 0x1f);
	return *this;
}

Irq& Irq::disable() {
	NVIC->ISER[ irq >> 5] &= ~(1<<(irq & 0x1f));
	return *this;
}

Irq& Irq::setPriority(int ip) {
	NVIC->IP[irq]=ip;
	return *this;
}
