#ifndef CENV_H
#define CENV_H
#include "Vertex.h"
#include <Vector3.h>
#include <transformation.h>
class Cenvironment
{
private:
	bool collided;
	float distance;
	float radius;
	float overlap;
	Position position;
	transform transformations;
	static Cenvironment* myObject;

public:
	Cenvironment();
	~Cenvironment();
	static Cenvironment* get_instance();

	bool getcollide() const;
	float getDistance() const;
	float getoverlap() const;
	transform get_transformation(void) const;

	void set_transformation(float angle, Vector3 vector);//rotation
	void set_transformation(char type, Vector3 vector);
	virtual void roundCollision(Cenvironment& object, float radius);
};
#endif
