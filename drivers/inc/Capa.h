#ifndef _CAPA_H
#define _CAPA_H

/** 
@brief Measure the capacitance of a load connected between a given GPIO and
the ground.

This works by loading the GPIO at a high state, then setting it as input. The
capacitor will slowly discharge through the GPIO, crossing the high to low
threshold after some time.

The longer the capacitor takes to cross that threshold, the larger the
capacitor. The measurement returned is the time the capacity took to
get to that threshold.

This is typically used to make touch operated controllers where the user
approaches his finger from a capacitor drawn on the PCB - this changes the
insulator permeability and thus the capacitor value.

This class can monitor at most 8 inputs at the same time.
*/
class Capa {
	private:
		int n;
		Gpio pins[8];
		int values[8];
		void prepare(int);
		int offset[8];
	public:
		Capa();
		Capa& add(Gpio);

		//Pre load capacitors
		Capa& prepare();
		//Actually do the measurement
		Capa& measure();
		Capa& update();

		//Use as a function to update
		inline void operator()() {
			update();
		}
		inline int operator[](int i) {
			return values[i];
		}
};

#endif /* _CAPA_H */
