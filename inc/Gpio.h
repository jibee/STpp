#ifndef _GPIO_H
#define _GPIO_H

class GpioPort;
class Gpio {
	public:
		enum Direction {
			INPUT,
			OUTPUT,
			ANALOG
		};
		void setDirection(Gpio::Direction);

		enum Speed {
			SPEED_100MHz,
			SPEED_50MHz,
			SPEED_25MHz,
			SPEED_2MHz
		};
		void setSpeed(Gpio::Speed);
		void setPushPull();
		void setOpenDrain();

		enum Function {
			GPIO,
			ALTERNATE
		};
		void setFunction(Gpio::Function);

		void setState(bool);
		bool getState();
		enum Resistor {
			NONE,
			PULL_UP,
			PULL_DOWN,
		};
		void setResistor(Resistor);
		enum AF {
			SYSTEM,
			TIM1_2,
			TIM3_5,
			TIM8_11,
			I2C1_3,
			SPI1_2,
			SPI_3,
			USART1_3,
			USART4_6,
			CAN1_2_TIM12_14,
			OTG_FS_HS,
			Eth,
			FSMC_SDIO_OTG_HS,
			Dcmi,
			AF_14,
			EVENT_OUT,
		};
		void setAlternate(int);
		void setAlternate(AF);
	private:
		GpioPort *port;
		int number;
		Gpio::Function function;
		Gpio::Direction direction;
		Gpio(GpioPort*, int);
		void updateModeR();
		friend class GpioPort;
		friend class Exti;
};

class GpioPort {
	private:
		volatile void *base;
	public:
		Gpio operator[](int n) {
			return Gpio(this, n);
		}
		GpioPort(volatile void* b);
		friend class Gpio;
		int getPortNumber();
};

extern GpioPort GpioA;
extern GpioPort GpioB;
extern GpioPort GpioC;
extern GpioPort GpioD;
extern GpioPort GpioE;
extern GpioPort GpioF;
extern GpioPort GpioG;
extern GpioPort GpioH;
extern GpioPort GpioI;

#endif /* _GPIO_H */
