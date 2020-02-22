#pragma once
#include "Cenvironment.h"
class Crectangle :
	public Cenvironment
{
private:
	float width;
	float height;
public:
	Crectangle();
	~Crectangle();
	void roundCollision(CPlayer p, float , float, float , float);
};

