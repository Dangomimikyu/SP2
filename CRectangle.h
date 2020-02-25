#pragma once
#include "CCollision.h"
#include "CCar.h"
class CRectangle : public CCollision
{
private:
	float width;
	float height;
public:
	CRectangle();
	~CRectangle();
	void RectCollision(CPlayer p,transform, float, float, float, float);
	void RectCollision(CCar, transform, float, float, float, float);
};

