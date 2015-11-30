#include <Irq.h>
#include "Asserv.h"

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

