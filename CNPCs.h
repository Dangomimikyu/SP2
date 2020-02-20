#ifndef CNPCS_H
#define CNPCS_H
#include <string>
#include <Vector3.h>
#include "Vertex.h"
#include <transformation.h>
#include "Cenvironment.h"

class NPC : public Cenvironment {
private:
	std::string name;
	bool idle;
	Vector3 walk_destination;
public:
	NPC();

	int activity(bool player);

	Vector3 get_walk(void) const;
	bool get_idle(void) const;

	void set_idle(void);
};


#endif // ! CNPCS_H
