#pragma once
#include "CCollision.h"
class CRectangle : public CCollision
{
private:
	float width;
	float height;
public:
	CRectangle();
	~CRectangle();
	void roundCollision(CPlayer p,transform, float, float, float, float);
};

