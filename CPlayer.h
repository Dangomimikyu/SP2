#pragma once
#include <transformation.h>
class CPlayer
{
private:
	transform transformations;
	float radius;
public:
	CPlayer();
	~CPlayer();
	void set_transformationR(float angle, Vector3 vector);//rotation
	void set_transformation(char type, Vector3 vector);
	void set_radius(float);
	transform get_transformation(void) const;
	float getRadius();
};

