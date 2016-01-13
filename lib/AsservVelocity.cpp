#include "Asserv.h"
#include <Log.h>

VelocityAccelPerEncoderCompute::VelocityAccelPerEncoderCompute() {
	*this = 0;
}

void VelocityAccelPerEncoderCompute::compute(int v) {
	int velocity = v - last;

	lastAccel = velocity*16 - lastVelocity;
	lastVelocity = velocity + lastVelocity*15/16;
	last = v;
}

int VelocityAccelPerEncoderCompute::getVelocity() const{
	return lastVelocity;
}

int VelocityAccelPerEncoderCompute::getAccel() const{
	return lastAccel;
}

int VelocityAccelPerEncoderCompute::getCurrent() const{
	return last;
}

VelocityAccelPerEncoderCompute::operator int() const{
	return last;
}

void VelocityAccelPerEncoderCompute::operator()(int v) {
	compute(v);
}

int VelocityAccelPerEncoderCompute::operator=(int v) {
	if(v != 0) while(1);
	last = lastVelocity = lastAccel = 0;
	return v;
}

VelocityAccel::VelocityAccel(VelocityAccelPerEncoderCompute &left, VelocityAccelPerEncoderCompute& right,
	IncrementalEncoder& eLeft, IncrementalEncoder& eRight, int N, int D) :
	left(left), right(right),
	eLeft(eLeft), eRight(eRight),
	intDist(0), intAngle(0),
	targetDist(0), targetAngle(0),
	N(N), D(D) {

	minLinearAccel = maxLinearAccel = 0;
	minRotationAccel = maxRotationAccel = 0;
	minVel = maxVel = 0;
}

void VelocityAccel::compute(int targetDist, int targetAngle) {
	eLeft.update();
	eRight.update();

	left.compute(eLeft);
	right.compute( (N*eRight)/D );

	if(this->targetDist != targetDist)
		intDist = 0;
	this->targetDist = targetDist;

	if(this->targetAngle != targetAngle)
		intAngle = 0;
	this->targetAngle = targetAngle;

	if(getAccelDist() > maxLinearAccel)
		maxLinearAccel = getAccelDist();
	if(getAccelDist() < minLinearAccel)
		minLinearAccel = getAccelDist();

	if(getAccelAngle() > maxRotationAccel)
		maxRotationAccel = getAccelAngle();
	if(getAccelAngle() < minRotationAccel)
		minRotationAccel = getAccelAngle();

	if(getVelocityDist() > maxVel)
		maxVel = getVelocityDist();
	if(getVelocityDist() < minVel)
		minVel = getVelocityDist();

	//Inversion of sign...
	//Cancel out integral
	if( (intAngle * getDeltaAngle()) < 0)
		intAngle = 0;
	else
		intAngle = getDeltaAngle() + intAngle*255/256;
	if( (intDist * getDeltaDist()) < 0) {
		Log::log << "Remise a 0 de l'integrale" <<endl;
		intDist = 0;
	} else
		intDist = getDeltaDist() + intDist*255/256;
}

int VelocityAccel::getIntegralAngle() const {
	return intAngle;
}

int VelocityAccel::getIntegralDist() const{
	return intDist;
}

int VelocityAccel::getAngle() const {
	return (left-right)/2;
}

int VelocityAccel::getDist() const {
	return (left+right)/2;
}

int VelocityAccel::getDeltaAngle() const{
	return targetAngle - getAngle();
}

int VelocityAccel::getDeltaDist() const {
	return targetDist - getDist();
}

int VelocityAccel::getVelocityAngle() const {
	return (left.getVelocity() - right.getVelocity())/2;
}

int VelocityAccel::getVelocityDist() const{
	return (left.getVelocity() + right.getVelocity())/2;
}

int VelocityAccel::getAccelAngle() const {
	return (left.getAccel() - right.getAccel())/2;
}

int VelocityAccel::getAccelDist() const {
	return (left.getAccel() + right.getAccel())/2;
}


OStream& operator<<(OStream& o, const VelocityAccel& i) {
    o << " Current Angle = " << i.getAngle() << endl;
    o << " Current Distance = " << i.getDist() << endl;
    o << " Target Angle = " << i.getTargetAngle() << endl;
    o << " Target Distance = " << i.getTargetDistance() << endl;
    o << " Delta Angle = " << i.getDeltaAngle() << endl;
    o << " Delta Distance = " << i.getDeltaDist() << endl;
    o << " Integral Angle = " << i.getIntegralAngle() << endl;
    o << " Integral Distance = " << i.getIntegralDist() << endl;
    o << endl;

    o << " Max Linear Accel = " << i.maxLinearAccel << endl;
    o << " Min Linear Accel = " << i.minLinearAccel << endl;

    o << " Max Rotation Accel = " << i.maxRotationAccel << endl;
    o << " Min Rotation Accel = " << i.minRotationAccel << endl;

    o << " Max Vel = " << i.maxVel << endl;
    o << " Min Vel = " << i.minVel << endl;
    return o;
}
