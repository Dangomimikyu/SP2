#include "CCollision.h"

CCollision::CCollision() : collided(false), distance(0), radius(0), overlap(0)
{
}

CCollision::~CCollision()
{
}

bool CCollision::getCollide() const
{
	return collided;
}

float CCollision::getDistance() const
{
	return distance;
}

float CCollision::getOverlap() const
{
	return overlap;
}

float CCollision::getRadius() const
{
	return radius;
}

transform CCollision::get_transformation() const
{
	return transformations;
}

void CCollision::set_transformation(float angle, Vector3 vector)
{
}

void CCollision::set_transformation(char type, Vector3 vector)
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

void CCollision::roundCollision(CPlayer p, transform t, float r)
{
	p.getRadius();
	distance = sqrt(pow(this->get_transformation().translation.x - t.translation.x, 2) + pow(this->get_transformation().translation.z - t.translation.z, 2));

	if (distance >= p.getRadius() + r)
	{
		collided = false;
	}
	else if (distance < r + p.getRadius())
	{
		collided = true;
		overlap = (p.getRadius() + r) - distance;
	}
}
