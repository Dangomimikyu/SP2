#include "CNPCs.h"

NPC::NPC() : idle(true), activity_number(-1), walk_destination(0, 0, 0)
{
	std::cout << "Creating new NPC" << std::endl;
	srand(time(0));
	activity(false);
}

void NPC::activity(bool player)
{
	if (player) {
		activity_number = 99;
		// insert code for player interaction
		return;
	}
	if (idle) {
		activity_number = rand() % 10;
		if (activity_number < 2) { // chill
			activity_name = "chilling";
			idle = false;
		}
		else if (activity_number >= 2 && activity_number <= 5) { // walking somewhere
			walk_destination.x = rand() % 50;
			walk_destination.y = 0;
			walk_destination.z = rand() % 50;
			std::cout << "going to: " << walk_destination.x << ", " << walk_destination.z << std::endl;
			activity_name = "walking";
			idle = false;
		}
		else if (activity_number > 5 && activity_number < 8) {// arcade cabinet
			int cabinet = rand() % 3;
			activity_name = "gaming";
			// go to cabinet
		}
		else if (activity_number >= 8) { // getting coffee
			// go to coffee machine
			activity_name = "getting coffee";
		}
		std::cout << activity_name << std::endl;
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

std::string NPC::get_activity(void) const
{
	return activity_name;
}

void NPC::set_idle(void)
{
	idle = !idle;
	std::cout << "setting idle as: " << idle << std::endl;
	activity(false);

}
