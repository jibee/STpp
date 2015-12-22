#ifndef _ASSERV_H
#define _ASSERV_H
#include <HBridgeST.h>
#include <IncrementalEncoder.h>
#include <OStream.h>
#include <Timer.h>
#include <Log.h>

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

template <class PWM_TIMER>
class Asserv: public _AsservBase {
    private:
	HBridgeST<PWM_TIMER>& motorl;
	HBridgeST<PWM_TIMER>& motorr;

    public:
	Asserv(IncrementalEncoder& left, IncrementalEncoder& right, Platform::Timer& tim, HBridgeST<PWM_TIMER>& mot1, HBridgeST<PWM_TIMER>& mot2);
	virtual Asserv<PWM_TIMER>& reset();

};


template <class TIMER_TYPE>
Asserv<TIMER_TYPE>& Asserv<TIMER_TYPE>::reset() {
    tim.disable();
    motorl.setSpeed(0);
    motorr.setSpeed(0);
    return *this;
}

template <class TIMER_TYPE>
Asserv<TIMER_TYPE>::Asserv(IncrementalEncoder& _left, IncrementalEncoder& _right,
    Platform::Timer& tim, HBridgeST<TIMER_TYPE> &mot1, HBridgeST<TIMER_TYPE> &mot2) :
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

#endif /* _ASSERV_H */
