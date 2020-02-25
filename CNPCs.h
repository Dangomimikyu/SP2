#ifndef CNPCS_H
#define CNPCS_H
#include <string>
#include <transformation.h>
#include <Mtx44.h>
#include <Vector3.h>
#include "CEntity.h"

class NPC : public CEntity
{
private:
	int replyChance;
	bool idle;
	int activity_number;
	Vector3 walk_destination;
	bool goodReply; // new
public:
	NPC();

	void activity(bool player);

	Vector3 get_walk(void) const;
	bool get_idle(void) const;
	int get_activity(void) const;
	bool get_goodReply(void) const; //new

	void set_idle(void);
};

#endif // !CNPCS_H
