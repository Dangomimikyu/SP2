#pragma once
#include "player.h"
#include "Vertex.h"
#include <Vector3.h>
#include <transformation.h>
#include "CPlayer.h"
class Cenvironment
{
private:
	Position position;
	transform transformations;
protected:
	bool collided;
	float distance;
	float radius;
	float overlap;
public:
	Cenvironment();
	~Cenvironment();
	bool getcollide() const;
	float getDistance() const;
	float getoverlap() const;
	transform get_transformation(void) const;

	void setPosition(float, float);
	void set_transformationR(float angle, Vector3 vector);//rotation
	void set_transformation(char type, Vector3 vector);
	virtual void roundCollision(CPlayer, float);
};

