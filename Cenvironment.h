#pragma once
#include "player.h"
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
public:
	Cenvironment();
	~Cenvironment();
	bool getcollide() const;
	float getDistance() const;
	float getoverlap() const;
	transform getTransformation(void) const;

	void setPosition(float, float);
	void set_transformationR(float angle, Vector3 vector);//rotation
	void set_transformation(char type, Vector3 vector);
	virtual void roundCollision(player, float);
};

