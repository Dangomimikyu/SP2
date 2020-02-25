#include "transformation.h"

transform transform::operator+(transform other) const
{
	return *this + other;
}

transform transform::operator-(transform other) const
{
	return *this - other;
}

transform transform::operator*(transform other) const
{
	return *this * other;
}

transform transform::operator/(transform other) const
{
	return *this / other;
}
