#pragma once
#include "CPlayer.h"
#include <Vector3.h>
#include <transformation.h>
class CCollision
{
private:
	transform transformations;
protected:
	bool collided;
	float distance;
	float radius;
	float overlap;
public:
	CCollision();
	~CCollision();
	bool getCollide() const;
	float getDistance() const;
	float getOverlap() const;
	float getRadius() const;
	transform get_transformation() const;

	void set_transformation(float angle, Vector3 vector);
	void set_transformation(char type, Vector3 vector);
	virtual void roundCollision(CPlayer, transform, float);
};

