#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Mtx44.h"
#include "Vector3.h"
struct Vector4
{
	float angle, x, y, z;
	Vector4(float angle = 0, float x = 0, float y = 0, float z = 0) { Set(angle, x, y, z); }
	void Set(float angle = 0, float x = 0, float y = 0, float z = 0) { this->angle = angle, this->x = x, this->y = y, this->z = z; }

	
};

struct transform {
	Vector3 translation;
	Vector4 rotationX = Vector4(0, 1, 0, 0);
	Vector4 rotationY = Vector4(0, 0, 1, 0);
	Vector4 rotationZ = Vector4(0, 0, 0, 1);
	Vector3 scaling = Vector3(1, 1, 1);

	transform operator+(transform other) const;
	transform operator-(transform other) const;
	transform operator*(transform other) const;
	transform operator/(transform other) const;

};

#endif // ! TRANSFORM_H
