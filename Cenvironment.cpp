#include "Cenvironment.h"

Cenvironment::Cenvironment() : collided(false), distance(0), radius(0), overlap(0)
{

}

Cenvironment::~Cenvironment()
{
}

Cenvironment* Cenvironment::get_instance()
{
	if (!myObject) {
		myObject = new Cenvironment();
		return myObject;
	}
	else {
		return myObject;
	}
}
Cenvironment* Cenvironment::myObject = 0;
void Cenvironment::roundCollision(Cenvironment& object, float r)
{
	radius = r;
	/// pythagoras' theorem to find the length
	//distance = sqrt(pow(this->get_transformation().translation.x - object.position.x, 2) + pow(this->get_transformation().translation.z - object.position.z, 2));
	distance = sqrt(pow(this->get_transformation().translation.x - get_instance()->get_transformation().translation.x, 2) + pow(this->get_transformation().translation.z - get_instance()->get_transformation().translation.z, 2));
	if (distance >= r + get_instance()->radius)
	{
		collided = false;
	}
	else if (distance < r + get_instance()->radius)
	{
		collided = true;
		//overlap = /*0.5 **/ (distance - p.radius - r);
		overlap = (get_instance()->radius + r) - distance;
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

void Cenvironment::set_transformation(float angle, Vector3 vector)
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
