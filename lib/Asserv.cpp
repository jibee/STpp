#include <Irq.h>
#include "Asserv.h"
#include <Shell.h>

using namespace Platform;

_AsservBase::_AsservBase(IncrementalEncoder& _left, IncrementalEncoder& _right,
    Timer& tim):
    tim(tim),
    eLeft(_left), eRight(_right),
    infos(left, right, eLeft, eRight, 1, 1),
    position(infos, 1000, 1000),
    targetAngle(0), targetDist(0),
    c_propDist(0x20000), c_propAngle(0x800),
    c_intDist(0), c_intAngle(0x12),
    c_velDist(0x0), c_velAngle(0),
    c_accelDist(0x0), c_accelAngle(0),
    maxEngine(0x3ff), minEngine(0x80),
    maxForwardAccel(0x80), maxBackwardAccel(0x80),
    maxRotationAccel(0x10000),
    waiting(false),
    date(0), dateStart(0)
{
    tim
        .setPrescaler(42)
        .setAutoReload(1000)
        .setOneShot(false)
        .setUIE(true)
        .setURS(true);

    Irq(tim.irqNr())
        .setPriority(15)
        .enable();

}


_AsservBase& _AsservBase::wait() {
	while(waiting);
	return *this;
}

_AsservBase& _AsservBase::setTargetDist(int t) {
	targetDist = t;

	dateStart = date;
	beenZero = 0;
	waiting = true;
	return *this;
}

_AsservBase& _AsservBase::setTargetAngle(int t) {
	targetAngle = t;

	dateStart = date;
	beenZero = 0;
	waiting = true;
	return *this;
}

_AsservBase& _AsservBase::setProportionnalDist(int c) {
	c_propDist = c;
	return *this;
}

_AsservBase& _AsservBase::setProportionnalAngle(int c) {
	c_propAngle = c;
	return *this;
}

_AsservBase& _AsservBase::setIntegralDist(int c) {
	c_intDist = c;
	return *this;
}

_AsservBase& _AsservBase::setIntegralAngle(int c) {
	c_intAngle = c;
	return *this;
}

_AsservBase& _AsservBase::setVelocityAngle(int c) {
	c_velAngle = c;
	return *this;
}

_AsservBase& _AsservBase::setVelocityDist(int c) {
	c_velDist = c;
	return *this;
}

_AsservBase& _AsservBase::setAccelDist(int c) {
	c_accelDist = c;
	return *this;
}

_AsservBase& _AsservBase::setAccelAngle(int c) {
	c_accelAngle = c;
	return *this;
}

_AsservBase& _AsservBase::setMaxForwardAcceleration(int l) {
	maxForwardAccel = l;
	return *this;
}

_AsservBase& _AsservBase::setMaxBackwardAcceleration(int l) {
	maxBackwardAccel = l;
	return *this;
}

_AsservBase& _AsservBase::setMaxRotationAcceleration(int l) {
	maxRotationAccel = l;
	return *this;
}

_AsservBase& _AsservBase::setMaxEngine(int l) {
	maxEngine = l;
	return *this;
}

_AsservBase& _AsservBase::setMinEngine(int l) {
	minEngine = l;
	return *this;
}


_AsservBase& _AsservBase::start() {
	left = right = 0;
	targetDist = targetAngle = 0;
	tim.enable();
	return *this;
}

int _AsservBase::getAngle() {
	return (left-right)/2;
}

int _AsservBase::getDist() {
	return (left+right)/2;
}

_AsservBase& _AsservBase::angle(int a) {
	setTargetAngle(targetAngle+a);
	return *this;
}

_AsservBase& _AsservBase::dist(int t) {
	setTargetDist(targetDist+t);
	return *this;
}

Position& _AsservBase::getPosition() {
	return position;
}

OStream& operator<<(OStream& o, const _AsservBase& a)
{
    o << "Asserv" << endl;
    o << a.getVelocityAccell();
    o << "Coefficients" << endl;
    o << "Dist.... Angle..." << endl;

    o << "     Proportional" << endl;
    o << a.c_propDist << " " << a.c_propAngle << endl;

    o << "     Integral" << endl;
    o << a.c_intDist << " " << a.c_intAngle << endl;

    o << "     Derivative" << endl;
    o << a.c_velDist << " " << a.c_velAngle << endl;

    o << "     Acceleration" << endl;
    o << a.c_accelDist << " " << a.c_accelAngle << endl;

    o << "     Position" << endl;
    o << a.position;
    return o;
}

Shell& operator<<(Shell& shell, _AsservBase& a) {
    if(!shell.got_name) while(1);

    addSetter(shell, a, setTargetAngle);
    addSetter(shell, a, setTargetDist);

    addSetter(shell, a, setVelocityAngle);
    addSetter(shell, a, setVelocityDist);
    addSetter(shell, a, setProportionnalAngle);
    addSetter(shell, a, setProportionnalDist);
    addSetter(shell, a, setIntegralAngle);
    addSetter(shell, a, setIntegralDist);
    addSetter(shell, a, setAccelAngle);
    addSetter(shell, a, setAccelDist);

    addSetter(shell, a, setMaxEngine);
    addSetter(shell, a, setMinEngine);
    addSetter(shell, a, setMaxForwardAcceleration);
    addSetter(shell, a, setMaxBackwardAcceleration);
    addSetter(shell, a, setMaxRotationAcceleration);

    addSetter(shell, a, angle);
    addSetter(shell, a, dist);

    shell.add([&a,&shell](Stack& s) {
        (void)s;
        a.reset();
    }, shell.current_object, "reset");

    shell.add([&a,&shell](Stack& s) {
        (void)s;
        a.start();
    }, shell.current_object, "start");

    shell.add([&a,&shell](Stack& s) {
        (void)s;
        OStream& o = *(shell.out);
        o << a;
    }, shell.current_object);

    shell.got_name = false;
    return shell;
}

Asserv& Asserv::reset() {
    tim.disable();
    motorl.setSpeed(0);
    motorr.setSpeed(0);
    return *this;
}

Asserv::Asserv(IncrementalEncoder& _left, IncrementalEncoder& _right,
    Platform::Timer& tim, HBridge &mot1, HBridge &mot2) :
    _AsservBase(_left, _right, tim),
    motorl(mot1), motorr(mot2)
{
    throttle = 100;


    tim
        .setTopCB([&tim, this](int timer_id) {
            (void)timer_id;
            ++date;
            infos.compute(targetDist, targetAngle);
            position.update();

            int d_d = 0, d_a = 0;
            //Distance
            d_d += c_accelDist * infos.getAccelDist();
            d_d += c_velDist * infos.getVelocityDist();
            d_d += c_propDist * infos.getDeltaDist();
            d_d += c_intDist * infos.getIntegralDist();

            //Angle
            d_a += c_accelAngle * infos.getAccelDist();
            d_a += c_velAngle * infos.getVelocityAngle();
            d_a += c_propAngle * infos.getDeltaAngle();
            d_a += c_intAngle * infos.getIntegralAngle();
#define abs(x) ((x) > 0 ? (x) : -(x))
#define signof(x, y) ((x) > 0 ? (y) : -(y))

#if 1
            //Check if we're near the destination (dist)
            int maxAccel = infos.getVelocityDist() > 0 ? maxBackwardAccel : maxForwardAccel;
            int x0 = (1000*infos.getVelocityDist()*infos.getVelocityDist())/(16*maxAccel);
            x0 = abs(x0);
            if(abs(infos.getDeltaDist()) < x0) {
                //Brrrrrrrrrrrrrrrrrrrrrrrrrraaaaaaaaaaakkkkkeeeeeeeee
                d_d = 0;
            }
#endif
#if 1
            //Check if we're near the destination (angle)
            int t0 = (1000*infos.getVelocityAngle()*infos.getVelocityAngle())/(16*maxRotationAccel);
            t0 = abs(t0);
            if(abs(infos.getDeltaAngle()) < t0) {
                //Brrrrrrrrrrrrrrrrrrrrrrrrrraaaaaaaaaaakkkkkeeeeeeeee
                d_a = 0;
            }
#endif

            int dl = d_d + d_a;
            int dr = d_d - d_a;

            dl/=0x4000;
            dr/=0x4000;

            if(abs(dl) > maxEngine || abs(dr) > maxEngine) {
                if(abs(dl) > abs(dr)) {
                    dr = signof(dr, abs(dr)-abs(dl)+maxEngine);
                    dl = signof(dl, maxEngine);
                } else {
                    dl = signof(dl, abs(dl)-abs(dr)+maxEngine);
                    dr = signof(dr, maxEngine);
                }
            }

            int tot = abs(dl)+abs(dr);

            if(tot < minEngine) {
                dl = 0;
                dr = 0;
            }

            //Check we're not blocked
            if(infos.getVelocityDist() == 0 && infos.getVelocityAngle() == 0) {
                beenZero++;
            } else
                beenZero = 0;

            //150ms
            if(beenZero > 150) {
                if(dateStart) {
                    Log::log << "At " << date << ", we finished command from " << dateStart << endl;
                    int res = date - dateStart;
                    Log::log << "That makes time of " << res/1000 << "s" << res%1000 << "ms" << endl;
                    dateStart = 0;
                    waiting = false;
                }
                dl = 0;
                dr = 0;
            }


            //ABS/ESP
            if(infos.getAccelDist() > maxForwardAccel || infos.getAccelDist() < -maxBackwardAccel) {
                if(throttle > 0) {
                    throttle -= 10;
                }
                Log::log << "Throttle at " << throttle << endl;
                Log::log << " Accel = " << infos.getAccelDist() << endl;
            } else {
                if(throttle < 100) {
                    Log::log << "Throttle at " << throttle << endl;
                    throttle += 10;
                }
            }
            int ref = 0;
#if 1
            if( (infos.getAccelDist() * infos.getVelocityDist()) > 0) {
                //We are accelerating
                //Throttling means reducing power
                ref = 0;
            } else if( (infos.getAccelDist() * infos.getVelocityDist()) < 0) {
                //We are braking
                //Throttling means increasing power

                ref = signof(infos.getVelocityDist(), 4096);
            }
#endif

            dl = ref * (100-throttle) + throttle * dl;
            dl /= 100;
            dr = ref * (100-throttle) + throttle * dr;
            dr /= 100;

            motorl.setSpeed(dl);
            motorr.setSpeed(dr);
        })
        .enable();
}

