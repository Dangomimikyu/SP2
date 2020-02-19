#include "Cenvironment.h"

Cenvironment::Cenvironment() : collided(false), distance(0), radius(0), overlap(0), positionX(0), positionZ(0)
{
	
}

Cenvironment::~Cenvironment()
{
}

void Cenvironment::collision(player p, float r)
{
	radius = r;
	distance = sqrt(pow(position.x - p.position.x, 2) + pow(position.z - p.position.z, 2));
	if (distance >= r + p.radius)
	{
		collided = false;
	}
	else if (distance < r + p.radius)
	{
		collided = true;
		//overlap = /*0.5 **/ (distance - p.radius - r);
		overlap = (p.radius + r) - distance;
	}
}

bool Cenvironment::getcollide()
{
	return collided;
}

float Cenvironment::getDistance()
{
	return distance;
}

float Cenvironment::getoverlap()
{
	return overlap;
}

void Cenvironment::setPosition(float x , float z)
{
	this->position.x = x;
	this->position.z = z;
}

float Cenvironment::getPositionX()
{
	return position.x;
}

float Cenvironment::getPositionZ()
{
	return position.z;
}
