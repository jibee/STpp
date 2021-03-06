#include <stm32f4xx.h>
#include "Gpio.h"
#include <Shell.h>
#include <stdlib.h>

using namespace Platform;

#define me ((volatile GPIO_TypeDef*)(this->port->base))

Gpio::Gpio(GpioPort* port, int n)
	: port(port), number(n) {
	function = GPIO;
	direction = INPUT;
}

Gpio& Gpio::setState(bool v) {
	if(v) {
		//Set pin
		me->BSRRL = 1 << number;
	} else {
		//Reset pin
		me->BSRRH = 1 << number;
	}
	return *this;
}

bool Gpio::operator=(bool v) {
	setState(v);
	return v;
}

#if 0
bool Gpio::getState() {
	return !! (me->IDR & (1 << number));
}
#endif

Gpio& Gpio::setSpeed(Gpio::Speed s) {
	int v = 0;
	switch(s) {
		case SPEED_2MHz:
			v = 0;
			break;
		case SPEED_25MHz:
			v = 1;
			break;
		case SPEED_50MHz:
			v = 2;
			break;
		case SPEED_100MHz:
			v = 3;
			break;
	};
	me->OSPEEDR = (me->OSPEEDR & ~(3 << (2*number))) | v << (2*number);
	return *this;
}

Gpio& Gpio::setPushPull() {
	me->OTYPER &= ~(1 << number);
	return *this;
}

Gpio& Gpio::setOpenDrain() {
	me->OTYPER |= 1 << number;
	return *this;
}

void Gpio::updateModeR() {
	if(direction==ANALOG)
		me->MODER = me->MODER | 3 << (2*number);
	else if(function != GPIO)
		me->MODER = (me->MODER & ~(3 << (2*number))) | 2 << (2*number);
	else if(direction==INPUT)
		me->MODER = me->MODER & ~(3 << (2*number));
	else
		me->MODER = (me->MODER & ~(3 << (2*number))) | 1 << (2*number);
}

Gpio& Gpio::setDirection(Gpio::Direction d) {
	direction = d;
	updateModeR();
	return *this;
}

Gpio& Gpio::setFunction(Gpio::Function f) {
	function = f;
	updateModeR();
	return *this;
}

GpioPort::GpioPort(volatile GPIO_TypeDef* b) : base(b) {
	//Get the number of the GPIO port based on base
	int n = getPortNumber();
	RCC->AHB1ENR |= 1 << n;
}

Gpio& Gpio::setResistor(Resistor r) {
	int v = 0;
	switch(r) {
		case NONE:
			v=0;
			break;
		case PULL_UP:
			v=1;
			break;
		case PULL_DOWN:
			v=2;
			break;
	};
	me->PUPDR = (me->PUPDR & ~(3<<(2*number))) | (v<<(2*number));
	return *this;
}

Gpio& Gpio::setAlternate(int af) {
	setFunction(ALTERNATE);
	int n = number;
	if(n>=8) {
		n-=8;
		me->AFR[1] = (me->AFR[1] & ~(0xf<<(4*n))) |
			af << (4*n);
	} else {
		me->AFR[0] = (me->AFR[0] & ~(0xf<<(4*n))) |
			af << (4*n);
	}
	return *this;
}

Gpio& Gpio::setAlternate(AF a) {
	setAlternate((int)a);
	return *this;
}

int GpioPort::getPortNumber() {
	int n = (((unsigned long)base) >> 10) & 0xf;
	return n;
}

Gpio& Gpio::operator=(Gpio& g) {
	number = g.number;
	port = g.port;
	function = g.function;
	direction = g.direction;

	return *this;
}

Gpio::Gpio() {
	number = -1;
}

Gpio& Gpio::setOutput()
{
    setFunction(Platform::Gpio::GPIO);
    setDirection(Platform::Gpio::OUTPUT);
    return  *this;
}

Gpio& Gpio::setInput(Platform::Gpio::Resistor weakResistor)
{
    setFunction(Platform::Gpio::GPIO);
    setDirection(Platform::Gpio::INPUT);
    setResistor(weakResistor);
    return *this;
}

int Gpio::getAdcChannel() const
{
    switch(port->getPortNumber())
    {
	case 0: /* port A */
	    {
		switch(number)
		{
		    case 0: return 0;
		    case 1: return 1;
		    case 2: return 2;
		    case 3: return 3;
		    case 4: return 4;
		    case 5: return 5;
		    case 6: return 6;
		    case 7: return 7;
		}
		break;
	    }
	case 1: /* port B */
	    {
		switch(number)
		{
		    case 0: return 8;
		    case 1: return 9;
		}
		break;
	    }
	case 2: /* port C */
	    {
		switch(number)
		{
		    case 0: return 10;
		    case 1: return 11;
		    case 2: return 12;
		    case 3: return 13;
		    case 4: return 14;
		    case 5: return 15;
		}
		break;
	    }
	case 6: /* port F */
	    {
		switch(number)
		{
		    case 3: return 9;
		    case 4: return 14;
		    case 5: return 15;
		    case 6: return 4;
		    case 7: return 5;
		    case 8: return 6;
		    case 9: return 7;
		    case 10: return 8;
		}
		break;
	    }

    }
    abort();
}


Shell& operator<<(Shell& shell, Platform::Gpio& g) {
    if(!shell.got_name) while(1);
    shell.add([&g,&shell](Stack& s) {
        (void)s;
        *shell.out << (g.getState() ? "# On" : "# Off" ) << endl;
    }, shell.current_object, "printState");

    shell.add([&g,&shell](Stack& s) {
        s.push( (g ? 1 : 0));
    }, shell.current_object, "state");

    shell.add([&g,&shell](Stack& s) {
        Object& o = s.pop();
        if(o.type != Object::INT) while(1);
        int v = o.toInt();

        g = v == 0 ? false : true;
    }, shell.current_object, "set");

    shell.got_name = false;
    return shell;
}



