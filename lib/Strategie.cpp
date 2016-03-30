#include "Strategie.h"
#include <Shell.h>

Strategie::Strategie(Ax12& Mamoutor, _AsservBase& a, RTOS::Time& t) :
    mamoutor_servo(Mamoutor), asserv(a), time(t) {

#if 1
	//If big robot
	asserv
	    .setProportionnalDist(0x1000)
	    //.setMaxForwardAcceleration(0x200)
	    //.setMaxBackwardAcceleration(0x200)
	    .setMaxForwardAcceleration(0x198)
	    .setMaxBackwardAcceleration(0x198)
	    .setMaxRotationAcceleration(0x68);
#else
	//If small robot
	asserv
	    .setProportionnalDist(0x20000)
	    .setMaxForwardAccel(0x80)
	    .setMaxBackwardAccel(0x80);
#endif

    }

void Strategie::run() {
    asserv.dist(10000);
    asserv.wait();

    asserv.angle(-2500);
    asserv.wait();

    asserv.dist(8000);
    asserv.wait();

    mamoutor();
    time.msleep(2000);
    resetMamoutor();
}

void Strategie::reset() {
    resetMamoutor();
    asserv.reset();

    time.msleep(500);
    mamoutor_servo.disable();
}

void Strategie::resetMamoutor() {
    mamoutor_servo.goTo(0x200);
}

void Strategie::mamoutor() {
    mamoutor_servo.goTo(0x300);
}

Shell& operator<<(Shell& shell, Strategie& strat) {
    if(!shell.got_name) while(1);

    shell.add([&strat](Stack& s) {
        (void) s;
        strat.run();
    }, shell.current_object, "run");

    shell.add([&strat](Stack& s) {
        (void) s;
        strat.mamoutor();
    }, shell.current_object, "mamoutor");

    shell.got_name = false;
    return shell;
}
