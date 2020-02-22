#include "CPlayer.h"

CPlayer::CPlayer() : radius(0)
{
}

float CPlayer::getRadius(void) const
{
	return radius;
}

void CPlayer::setRadius(float r)
{
	radius = r;
}
