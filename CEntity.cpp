#include "CEntity.h"

CEntity::CEntity() : name("bob")
{
}
#ifndef CENTITY_H
#define CENTITY_H
#include <string>
#include <Mtx44.h>
#include <transformation.h>

class CEntity
{
private:
	std::string name;
public:
	CEntity();
};

#endif // !CENTITY_H
