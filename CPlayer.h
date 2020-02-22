#ifndef CPLAYER_H
#define CPLAYER_H
#include "CEntity.h"
#include "Vertex.h"

class CPlayer : public CEntity
{
private:
	float radius;
public:
	CPlayer();
	float getRadius(void) const;
	void setRadius(float r);
};
#endif // !CPLAYER_H