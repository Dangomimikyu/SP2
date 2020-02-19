#include "CNPCs.h"
#include <ctime>

NPC::NPC() : name("bob"), idle(true)
{
	std::cout << "N E W  N P C"; // testing
	srand(time(0));
	activity();
}

int NPC::activity(bool player)
{
	int activity_number = 0;
	if (player) {
		activity_number = 99;
		// insert code for player interaction
	}
	if (idle) {
		activity_number = rand() % 3;
		switch (activity_number) {
		case 0: // npc is talking to another NPC
			std::cout << "talking";
			idle = false;
			break;
		case 1: // npc is walking somewhere
			walk_destination.x = rand() % 10;
			walk_destination.y = 0;
			walk_destination.z = rand() % 10;
			std::cout << "going to: " << walk_destination.x << ", " << walk_destination.z;
			idle = false;
			break;
		case 2: // npc wants to play arcade game
			int cabinet = rand() % 3;
			// go to cabinet location
		}
	}
	return activity_number;
}

Vector3 NPC::get_walk(void) const
{
	return walk_destination;
}

bool NPC::get_idle(void) const
{
	return idle;
}

transform NPC::get_transformation(void) const
{
	return transformations;
}
void NPC::set_transformation(float angle, Vector3 vector)
{
	transformations.rotateAngle = angle;
	transformations.rotation = vector;
}
void NPC::set_transformation(char type, Vector3 vector)
{
	if (type == 't') { // translate
		transformations.translation = vector;
	}
	else if (type == 's') { // scale
		transformations.scaling = vector;
	}
	else {
		throw ("you entered something that is not translate or scale");
	}
}
void NPC::set_idle(void)
{
	std::cout << "setting idle as: " << idle << std::endl;
	idle = !idle;
	activity(false);
}
