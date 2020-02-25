#include "CRectangle.h"

CRectangle::CRectangle() : width(0), height(0)
{
}

CRectangle::~CRectangle()
{
}

void CRectangle::roundCollision(CPlayer p, transform t, float minX, float minZ, float w, float h)
{
	width = w;
	height = h;
	float maxX = minX + w;
	float maxZ = minZ + h;
	/*this->set_transformation('t', Vector3((minX + w) / 2, 0, (minZ + h) / 2));*/
	if (/*p.get_transformation().translation.x < minX && p.get_transformation().translation.z < minZ*/t.translation.x < minX && t.translation.z < minZ)
	{
		this->set_transformation('t', Vector3(minX, 0, minZ));
	}
	if (t.translation.x < minX && t.translation.z > minZ && t.translation.z < maxZ)
	{
		this->set_transformation('t', Vector3(minX, 0, /*p.get_transformation().translation.z*/t.translation.z));
	}
	if (/*p.get_transformation().translation.x < minX && p.get_transformation().translation.x > maxZ*/t.translation.x < minX && t.translation.x > maxZ)
	{
		this->set_transformation('t', Vector3(minX, 0, maxZ));
	}
	if (t.translation.x >= minX && t.translation.x < maxX && t.translation.z < minZ)
	{
		this->set_transformation('t', Vector3(t.translation.x, 0, minZ));
	}
	if (t.translation.x >= minX && t.translation.x < maxX && t.translation.z > maxZ)
	{
		this->set_transformation('t', Vector3(t.translation.x, 0, maxZ));
	}
	if (t.translation.x > maxX&& t.translation.z < minZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, minZ));
	}
	if (t.translation.x > maxX && t.translation.z > minZ && t.translation.z < maxZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, t.translation.z));
	}
	if (t.translation.x > maxX && t.translation.z > maxZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, maxZ));
	}
	distance = sqrt(pow(t.translation.x - this->get_transformation().translation.x, 2) + pow(t.translation.z - this->get_transformation().translation.z, 2));
	if (distance >= p.getRadius())
	{
		collided = false;
	}
	else if (distance < p.getRadius())
	{
		collided = true;
	}
	overlap = p.getRadius() - distance;
}
