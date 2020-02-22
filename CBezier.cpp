#include "CBezier.h"

CBezier::CBezier():temp(0)
{
}

CBezier::~CBezier()
{
}

void CBezier::roundCollision(transform p0, transform p1, transform p2, transform p3, CPlayer p, transform t)
{
	for (float i = 0; i < 1; i += 0.01)
	{
		//cubic bezier curve 
		finalP.translation.x = pow(1 - i, 3) * p0.translation.x + pow(1 - i, 2) * 3 * i *
			p1.translation.x + (1 - i) * 3 * i * i * p2.translation.x + i * i * i * p3.translation.x;

		finalP.translation.z = pow(1 - i, 3) * p0.translation.z + pow(1 - i, 2) * 3 * i *
			p1.translation.z + (1 - i) * 3 * i * i * p2.translation.z + i * i * i * p3.translation.z;

		temp = sqrt(pow(t.translation.x - finalP.translation.x, 2) + pow(t.translation.z - finalP.translation.z, 2));
		if (distance == 0)
		{
			distance = temp;
		}
		else if (distance > temp)
		{
			distance = temp;
		}
		else if (distance < temp)
		{
			distance = distance;
		}
	}
	if (p.getRadius() < distance)
	{
		collided = false;
	}
	else if (p.getRadius() > distance)
	{
		collided = true;
		overlap = p.getRadius() - distance;
	}
}
