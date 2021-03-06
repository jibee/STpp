#include <tr1/functional>
#include <Capa.h>
#include <Log.h>
#include <Exti.h>

using namespace Platform;

Capa::Capa(RTOS::Time& highresTime, Platform::Timer& hwTimer) : n(0), time(highresTime), tim7(hwTimer) {
    tim7
	.setPrescaler(0x0)
	.setAutoReload(20000)
	.setCounter(0x0)
	.setOneShot(true)
	.update();
}

Capa& Capa::add(Gpio g) {
    int i = n;
    pins[n] = g;
    ++n;

    Exti(g)
	.enableFalling()
	.setTopCB([=](int) {
		values[i] = tim7.getCounter();
		prepare(i);
		})
    .enableIRQ();

    return *this;
}

void Capa::prepare(int i) {
    pins[i]
    .setFunction(Gpio::GPIO)
    .setDirection(Gpio::OUTPUT)
    .setSpeed(Gpio::SPEED_100MHz)
    .setState(true);
}

Capa& Capa::prepare() {
    int i;
    for(i=0;i<n;++i)
	prepare(i);

    return *this;
}

Capa& Capa::measure() {
    int i;
    tim7
	.setCounter(0)
	.setOneShot(true)
	.update()
	.enable();

    //Set pins to input
    for(i=0;i<n;++i) {
	pins[i]
	.setDirection(Gpio::INPUT)
	.setResistor(Gpio::NONE);
	values[i] = 0;
	offset[i] = tim7.getCounter();
    }

    while(tim7.enabled()) {
	int i;
	int failed = 0;
	for(i=0;i<n;++i) {
	    if(!values[i])
		failed = 1;
	}
	if(!failed)
	    break;
    }
    if(!tim7.enabled())
	Log::log << "Timed out" << endl;

    tim7
	.setCounter(0)
	.disable();
    for(i=0;i<n;++i) {
	values[i]-=offset[i];
    }
    return *this;
}

Capa& Capa::update() {
    prepare();
    time.msleep(10);
    measure();
    return *this;
}
