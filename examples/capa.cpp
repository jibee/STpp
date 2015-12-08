#include <tr1/functional>

#include <Ax12.h>
#include <Board.h>
#include <Capa.h>
#include <Exti.h>
#include <Log.h>
#include <Mean.h>
#include <Uart.h>
#include <Usb.h>
#include <Watchdog.h>
#include <Board.h>


int main() {
	log << "startup" << endl;
    Board b;
    Capa capa(b.time, b.Tim7);
	capa
		.add(b.GpioD[0])
		.add(b.GpioD[1])
		.add(b.GpioD[2]);

	capa();

	Mean slow[3]={Mean(4), Mean(4), Mean(4)};
	Mean fast[3]={Mean(1), Mean(1), Mean(1)};

	b.LedO.setDutyCycle(0);
	b.LedG.setDutyCycle(0);
	b.LedB.setDutyCycle(0);
	b.LedR.setDutyCycle(0);
	int len[3] = {0, 0, 0};
	while(1) {

		capa();

		int i;
		for(i=0;i<3;++i) {
			slow[i]+=capa[i];
			fast[i]+=capa[i];
		}
		b.LedB.setDutyCycle(0);
		if(capa[1] > 14800)
			b.LedB.setDutyCycle(100);


		for(i=0;i<3;++i) {
			int d;
			d = fast[i] - slow[i];
			if(d>15) {
				switch(i) {
					case 0:
						b.LedG.setDutyCycle(100);
						break;
					case 1:
						//b.LedO.setDutyCycle(100);
						break;
					case 2:
						//b.LedR.setDutyCycle(100);
						break;
				};
			}
			if(d<-20) {
				++len[i];
				log << "Fast " << fast[i];
				log << " Slow " << slow[i];
				log << " Delta " << d;
				log << " Len " << len[i];
				log << " Instant " << capa[i] << endl;
				switch(i) {
					case 0:
						b.LedG.setDutyCycle(0);
						break;
					case 1:
						//LedO.setDutyCycle(0);
						break;
					case 2:
						b.LedR.setDutyCycle(0);
						break;
				};
			} else {
				if(len[i]) {
					log << "Fast " << fast[i];
					log << " Slow " << slow[i];
					log << " Delta " << d;
					log << " Len " << len[i];
					log << " Instant " << capa[i] << endl;
				}
				len[i] = 0;
			}
		}
	}

	for(;;);
}
