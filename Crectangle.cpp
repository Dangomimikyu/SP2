#include "Crectangle.h"

Crectangle::Crectangle() : width(0), height(0)
{
}

Crectangle::~Crectangle()
{
}

void Crectangle::roundCollision(CPlayer p,float minX, float minZ, float w, float h)
{
	width = w;
	height = h;
	float maxX = minX + w;
	float maxZ = minZ + h;
	this->set_transformation('t', Vector3((minX + w) / 2, 0, (minZ + h) / 2));
	if (p.get_transformation().translation.x < minX && p.get_transformation().translation.z < minZ)
	{
		this->set_transformation('t', Vector3(minX, 0, minZ));
	}
	if (p.get_transformation().translation.x < minX && p.get_transformation().translation.z > minZ && p.get_transformation().translation.z < maxZ)
	{
		this->set_transformation('t', Vector3(minX, 0, p.get_transformation().translation.z));
	}
	if (p.get_transformation().translation.x < minX && p.get_transformation().translation.x > maxZ)
	{
		this->set_transformation('t', Vector3(minX, 0, maxZ));
	}
	if (p.get_transformation().translation.x >= minX && p.get_transformation().translation.x < maxX && p.get_transformation().translation.z < minZ)
	{
		this->set_transformation('t', Vector3(p.get_transformation().translation.x, 0, minZ));
	}
	if (p.get_transformation().translation.x >= minX && p.get_transformation().translation.x < maxX && p.get_transformation().translation.z > maxZ)
	{
		this->set_transformation('t', Vector3(p.get_transformation().translation.x, 0, maxZ));
	}
	if (p.get_transformation().translation.x > maxX && p.get_transformation().translation.z < minZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, minZ));
	}
	if (p.get_transformation().translation.x > maxX && p.get_transformation().translation.z > minZ && p.get_transformation().translation.z < maxZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, p.get_transformation().translation.z));
	}
	if (p.get_transformation().translation.x > maxX && p.get_transformation().translation.z > maxZ)
	{
		this->set_transformation('t', Vector3(maxX, 0, maxZ));
	}
	distance = sqrt(pow(p.get_transformation().translation.x - this->get_transformation().translation.x, 2) + pow(p.get_transformation().translation.z - this->get_transformation().translation.z, 2));
	if (distance >= p.getRadius())
	{
		collided = false;
	}
	else if(distance < p.getRadius())
	{
		collided = true;
	}
	overlap = p.getRadius() - distance;

}
