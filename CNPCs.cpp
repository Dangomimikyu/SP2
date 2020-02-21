#include "CNPCs.h"
#include <ctime>

NPC::NPC() : name("bob"), idle(true), activity_number(-1)
{
	std::cout << "!new npc!"; // testing
	srand(time(0));
	activity(false);
}

void NPC::activity(bool player)
{
	if (player) {
		activity_number = 99;
		// insert code for player interaction
	}
	if (idle) {
		activity_number = rand() % 3;
		switch (activity_number) {
		case 0: // npc is talking to another NPC
			std::cout << "talking" << std::endl;
			idle = false;
			break;
		case 1: // npc is walking somewhere
			walk_destination.x = rand() % 50;
			walk_destination.y = 0;
			walk_destination.z = rand() % 50;
			std::cout << "going to: " << walk_destination.x << ", " << walk_destination.z << std::endl;
			idle = false;
			break;
		case 2: // npc wants to play arcade game
			int cabinet = rand() % 3;
			// go to cabinet location
		}
	}
}

Vector3 NPC::get_walk(void) const
{
	return walk_destination;
}

bool NPC::get_idle(void) const
{
	return idle;
}

int NPC::get_activity(void) const
{
	return activity_number;
}

void NPC::set_idle(void)
{
	std::cout << "setting idle as: " << idle << std::endl;
	idle = !idle;
	activity(false);
}
