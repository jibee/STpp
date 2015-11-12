#ifndef _GPIO_H
#define _GPIO_H
#include <stm32f4xx.h>
/** Platform drivers 
 * 
 * 
 */
namespace Platform
{
class GpioPort;
/** Represent a single IO in a GPIO port */
class Gpio {
    public:
	/** Empty constructor to support RHS assignment */
	Gpio();
	/** Assignment operator - merely copies the IO port definition */
	Gpio& operator=(Gpio&);
	/** set the output state via a RHS assignement from boolean */
	bool operator=(bool);
	/** IO direction */
	enum Direction {
	    /** Input */
	    INPUT,
	    /** Output */
	    OUTPUT,
	    /** Analog input or output */
	    ANALOG
	};
	/** Sets the IO direction - input output or the special analog case */
	Gpio& setDirection(Gpio::Direction);
	/** IO Speed / slew rate control */
	enum Speed {
	    SPEED_100MHz,
	    SPEED_50MHz,
	    SPEED_25MHz,
	    SPEED_2MHz
	};
	/** Set the IO slew rate */
	Gpio& setSpeed(Gpio::Speed);
	/** Set the IO as push-pull */
	Gpio& setPushPull();
	/** Set the IO as open drain */
	Gpio& setOpenDrain();
	/** Function types for OUTPUT and INPUT directions */ 
	enum Function {
	    /** control and readback via the GPIO boolean value */
	    GPIO,
	    /** control from a specific hardware function */
	    ALTERNATE
	};
	/** Sets the control logic for INPUT and OUTPUT directions */
	Gpio& setFunction(Gpio::Function);
	/** When Direction=INPUT and Function=GPIO, sets the output to the given value */
	Gpio& setState(bool);
	/** Obtains the current output value */
	bool getState();
	/** Output weak resistor - pull up, pull down or none */
	enum Resistor {
	    /** no resistor */
	    NONE,
	    /** pull up */
	    PULL_UP,
	    /** pull down*/
	    PULL_DOWN,
	};
	/** Sets the output resistor
	  @param Resistor resistor specification
	 */
	Gpio& setResistor(Resistor);
	/** Alternative function choice */
	enum AF {
	    SYSTEM=0,
	    TIM1_2=1,
	    TIM3_5=2,
	    TIM8_11=3,
	    I2C1_3=4,
	    SPI1_2=5,
	    SPI_3=6,
	    USART1_3=7,
	    USART4_6=8,
	    CAN1_2_TIM12_14=9,
	    OTG_FS_HS=10,
	    Eth=11,
	    FSMC_SDIO_OTG_HS=12,
	    Dcmi=13,
	    AF_14=14,
	    EVENT_OUT=15,
	};
    /** When Function is Alternate, connects the specified function */
	Gpio& setAlternate(int);
    /** When Function is Alternate, connects the specified function */
	Gpio& setAlternate(AF);
	/** Obtains the current output value */
	inline operator bool() { return getState(); };
    private:
    /** GpioPort this pin belongs to */
	GpioPort *port;
	/** Pin number of this Gpio within the GpioPort */
	int number;
	/** Function of this pin */
	Gpio::Function function;
	/** Direction */
	Gpio::Direction direction;
	/** Constructor
     * 
     * @param port Gpio port this pin belongs to
     * @param n pin number within the Gpio port
     * 
     * note this constructor is only to be used from the GpioPort class - see below.
     */
	Gpio(GpioPort* port, int n);
	/**
     * Update the mode register from the current values of function and direction
     * 
     */
	void updateModeR();
	friend class GpioPort;
	friend class Exti;
};
/** GPIO Port / Bank */
class GpioPort {
    private:
	/** Where the control registers of the GPIO live. */
	volatile GPIO_TypeDef *base;
    public:
	/** Obtains the n-th IO pin in the port
	  @return a GPIO control object
	  @warning the resulting Gpio object will not reflect the actual configuration of the GPIO
	  through a read-back of the registers.
	 */
	Gpio operator[](int n) {
	    return Gpio(this, n);
	}
	/** Constructor

	  @param b the base address for the GPIO port control registers
	 */
	GpioPort(volatile GPIO_TypeDef* b);
	friend class Gpio;
	int getPortNumber();
};

/** Static GPIO ports definitions. */
extern GpioPort GpioA;
extern GpioPort GpioB;
extern GpioPort GpioC;
extern GpioPort GpioD;
extern GpioPort GpioE;
extern GpioPort GpioF;
extern GpioPort GpioG;
extern GpioPort GpioH;
extern GpioPort GpioI;

inline bool Gpio::getState() { return !!(port->base->IDR&(1<<number)); };
}
#endif /* _GPIO_H */
