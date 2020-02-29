#pragma once

#define CAR_MOD 3
class CCar
{
private:
	float speed;
	float acceleration;
	float radius;
	int tyre = 1;
	float modifier[CAR_MOD];

public:
	CCar();
	~CCar();
	void setSpeed(float s);
	void setAcceleration(float);
	void setRadius(float r);
	
	float getSpeed();
	float getAcceleration();
	float getRadius();
	float getModifier();

};

