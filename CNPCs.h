#ifndef CNPCS_H
#define CNPCS_H
#include <string>
#include <Vector3.h>
#include "Vertex.h"
#include <transformation.h>

class NPC {
private:
	std::string name;
	bool idle;
	Vector3 walk_destination;
	transform transformations;
public:
	NPC();

	int activity(bool player);

	Vector3 get_walk(void) const;
	bool get_idle(void) const;
	transform get_transformation(void) const;

	void set_transformation(float angle, Vector3 vector); // for rotate
	void set_transformation(char type, Vector3 vector); // for translate and scale
	void set_idle(void);
};


#endif // ! CNPCS_H
