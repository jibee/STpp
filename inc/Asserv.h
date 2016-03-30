#ifndef _ASSERV_H
#define _ASSERV_H
#include <HBridge.h>
#include <IncrementalEncoder.h>
#include <OStream.h>
#include <Timer.h>
#include <Log.h>

class Shell;

class VelocityAccelPerEncoderCompute {
    private:
	int last;
	int lastVelocity;
	int lastAccel;

    public:
	VelocityAccelPerEncoderCompute();
	void compute(int value);
	int getVelocity() const;
	int getAccel() const;
	int getCurrent() const;
	operator int() const;
	void operator()(int);
	int operator=(int);
};

class VelocityAccel {
    private:
	VelocityAccelPerEncoderCompute &left, &right;
	IncrementalEncoder &eLeft, &eRight;
	int intDist, intAngle;
	int targetDist, targetAngle;

	int N, D;
    public:
	VelocityAccel(
		VelocityAccelPerEncoderCompute& left,
		VelocityAccelPerEncoderCompute &right,
		IncrementalEncoder& eLeft,
		IncrementalEncoder& eRight,
		int N_LR,
		int D_LR
		);
	void compute(int dist, int angle);

	int getIntegralAngle() const;
	int getIntegralDist() const;

	int getTargetAngle() const{ return targetAngle;}
	int getAngle() const;
	int getDeltaAngle() const;
	int getDist() const;
	int getDeltaDist() const;

	int getVelocityAngle() const;
	int getVelocityDist() const;

	int getAccelAngle() const;
	int getAccelDist() const;
	friend OStream& operator<<(OStream&, const VelocityAccel&);
	int getTargetDistance() const{ return targetDist;}
	//friend class Asserv;
    public:
	int maxLinearAccel, minLinearAccel;
	int maxRotationAccel, minRotationAccel;
	int maxVel,minVel;
};

class Position {
    private:
	VelocityAccel& infos;
	float x;
	float y;
	float theta;
	int previousEDist;
	float wheelSize, wheelDist;
    public:
	Position(VelocityAccel& infos, float wheelSize, float wheelDist);
	void update();
	float getX();
	float getY();
	float getTheta();
	friend OStream& operator<<(OStream&, const Position&);
};

class _AsservBase
{
    protected:
	_AsservBase(IncrementalEncoder& _left, IncrementalEncoder& _right, Platform::Timer& tim);
	Platform::Timer& tim;
	IncrementalEncoder& eLeft;
	IncrementalEncoder& eRight;

	VelocityAccelPerEncoderCompute left;
	VelocityAccelPerEncoderCompute right;

	VelocityAccel infos;
	Position position;

	int targetAngle, targetDist;
	int c_propDist, c_propAngle;
	int c_intDist, c_intAngle;
	int c_velDist, c_velAngle;
	int c_accelDist, c_accelAngle;

	int maxEngine, minEngine;
	int maxForwardAccel, maxBackwardAccel;
	int maxRotationAccel;

	volatile bool waiting;
	int beenZero;

	int throttle;
	int date;
	int dateStart;
    public:
	const VelocityAccel& getVelocityAccell() const { return infos; }
	_AsservBase& wait();
	_AsservBase& setTargetDist(int t);
	_AsservBase& setTargetAngle(int a);
	_AsservBase& setProportionnalDist(int c);
	_AsservBase& setProportionnalAngle(int c);
	_AsservBase& setIntegralDist(int c);
	_AsservBase& setIntegralAngle(int c);
	_AsservBase& setVelocityDist(int c);
	_AsservBase& setVelocityAngle(int c);
	_AsservBase& setAccelDist(int c);
	_AsservBase& setAccelAngle(int c);
	_AsservBase& setMaxForwardAcceleration(int l);
	_AsservBase& setMaxBackwardAcceleration(int l);
	_AsservBase& setMaxRotationAcceleration(int l);
	_AsservBase& setMaxEngine(int l);
	_AsservBase& setMinEngine(int l);

	_AsservBase& angle(int a);
	_AsservBase& dist(int t);
	Position& getPosition();



	_AsservBase& start();
	virtual _AsservBase& reset()=0;

	int getAngle();
	int getDist();
	friend OStream& operator<<(OStream& o, const _AsservBase& a);
};

Shell& operator<<(Shell&, _AsservBase&);

class Asserv: public _AsservBase {
    private:
	HBridge& motorl;
	HBridge& motorr;

    public:
	Asserv(IncrementalEncoder& left, IncrementalEncoder& right, Platform::Timer& tim, HBridge& mot1, HBridge& mot2);
	virtual Asserv& reset();

};



#endif /* _ASSERV_H */
