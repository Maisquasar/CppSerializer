#pragma once
#include "CppSerializer.h"

class Vec2f
{
public:
	Vec2f(float x = 0, float y = 0) : x(x), y(y) {}

	float x;
	float y;

	std::string ToString() const;
};