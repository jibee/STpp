#include "Asserv.h"

extern "C" void arm_sin_cos_f32(float theta, float* sin, float* cos);
//wheelSize is in ticks/m
Position::Position(VelocityAccel& infos, float wheelSize, float wheelDist)
	:infos(infos), wheelSize(wheelSize), wheelDist(wheelDist) {
	x = y = 0;
	previousEDist = 0;
}

/*
                     x > 0
		     ^
                     |
                     |
                     |
                     |
                     |
 y> 0 <--------------i
 */
# define M_PI		3.14159265358979323846
void Position::update() {
	int eDeltaDist = infos.getDist() - previousEDist;
	float sin,cos;
	arm_sin_cos_f32(theta, &sin, &cos);
	float deltaDist = eDeltaDist / wheelSize;
	float dx = deltaDist * cos;
	float dy = deltaDist * sin;

	theta = infos.getAngle() / (wheelSize*wheelDist);
	//ARM cos/sin function are meant for degrees...
	theta *= 180.0/M_PI;
	x += dx;
	y += dy;
	previousEDist = infos.getDist();
}

OStream& operator<<(OStream& o, const Position& p) {
    o << "X = " << (int)(p.x*1000) << " mm" << endl;
    o << "Y = " << (int)(p.y*1000) << " mm" << endl;
    o << "Theta = " << (int)(p.theta*10) << " deciDegres" << endl;
    return o;
}
