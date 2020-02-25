#pragma once
#include "CCollision.h"
class CBezier :
	public CCollision
{
private:
	transform finalP;
	float temp;
public:
	CBezier();
	~CBezier();
	void roundCollision(transform, transform, transform, transform, CPlayer, transform);
};

