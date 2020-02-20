#include "Cenvironment.h"

Cenvironment::Cenvironment() : collided(false), distance(0), radius(0), overlap(0)
{

}

Cenvironment::~Cenvironment()
{
}

void Cenvironment::roundCollision(CPlayer p, float r) 
{
	radius = r;
	//pythagoras theorem to find the length//
	distance = sqrt(pow(this->get_transformation().translation.x - p.get_transformation().translation.x, 2) + pow(this->get_transformation().translation.z - p.get_transformation().translation.z, 2) );
	if (distance >= r + p.getRadius())
	{
		collided = false;
	}
	else if (distance < r + p.getRadius())
	{
		collided = true;
		//overlap = /*0.5 **/ (distance - p.radius - r);
		overlap = (p.getRadius() + r) - distance;
	}
}

bool Cenvironment::getcollide() const
{
	return collided;
}

float Cenvironment::getDistance() const
{
	return distance;
}

float Cenvironment::getoverlap() const
{
	return overlap;
}

void Cenvironment::setPosition(float x, float z)
{
	this->position.x = x;
	this->position.z = z;
}

void Cenvironment::set_transformationR(float angle, Vector3 vector)
{
	transformations.rotateAngle = angle;
	transformations.rotation = vector;
}

void Cenvironment::set_transformation(char type, Vector3 vector)
{
	if (type == 't') { // translate
		transformations.translation = vector;
	}
	else if (type == 's') { // scale
		transformations.scaling = vector;
	}
	else {
		throw ("you entered something that is not translate or scale");
	}
}

transform Cenvironment::get_transformation(void) const
{
	return transformations;

}
