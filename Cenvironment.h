#ifndef CENV_H
#define CENV_H
#include "player.h"
#include "Vertex.h"
class Cenvironment
{
private:
	bool collided;
	float distance;
	float radius;
	float overlap;
	float positionX;
	float positionZ;
	Position position;
public:
	Cenvironment();
	~Cenvironment();
	virtual void collision(player, float);
	bool getcollide();
	float getDistance();
	float getoverlap();
	void setPosition(float, float);
	float getPositionX();
	float getPositionZ();
};
#endif // ! CENV_H
