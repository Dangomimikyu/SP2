#include "Cenvironment.h"

Cenvironment::Cenvironment() : collided(false), distance(0), radius(0), overlap(0)
{

}

Cenvironment::~Cenvironment()
{
}

void Cenvironment::roundCollision(player p, float r) 
{
	radius = r;
	//pythagoras theorem to find the length//
	distance = sqrt(pow(this->getTransformation().translation.x - p.position.x, 2) + pow(this->getTransformation().translation.z - p.position.z, 2) );
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

transform Cenvironment::getTransformation(void) const
{
	return transformations;

}
