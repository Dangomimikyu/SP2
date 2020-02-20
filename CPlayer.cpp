#include "CPlayer.h"

CPlayer::CPlayer():radius(0)
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::set_transformationR(float angle, Vector3 vector)
{
	transformations.rotateAngle = angle;
	transformations.rotation = vector;
}

void CPlayer::set_transformation(char type, Vector3 vector)
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

void CPlayer::set_radius(float r)
{
	radius = r;
}

transform CPlayer::get_transformation(void) const
{
	return transformations;
}

float CPlayer::getRadius()
{
	return radius;
}
