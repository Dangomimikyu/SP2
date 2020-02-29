#include "CCar.h"

CCar::CCar() : speed(10), acceleration(1), radius(2)
{
	for (int i = 0; i < CAR_MOD; ++i) {
		modifier[i] = (i + 1) * 5;
	}
}

CCar::~CCar()
{
}

void CCar::setSpeed(float s)
{
	speed = s + acceleration;
}

void CCar::setAcceleration(float a)
{
	acceleration = a;
}

void CCar::setRadius(float r)
{
	radius = r;
}

float CCar::getSpeed()
{
	return speed;
}

float CCar::getAcceleration()
{
	return acceleration;
}

float CCar::getRadius()
{
	return radius;
}

float CCar::getModifier()
{
	return 5;
}
