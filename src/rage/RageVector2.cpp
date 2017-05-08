#include "RageVector2.hpp"
#include <cmath>

Rage::Vector2::Vector2() : x(0), y(0)
{
}

Rage::Vector2::Vector2(float a, float b) : x(a), y(b)
{
}

Rage::Vector2 & Rage::Vector2::operator+=(Rage::Vector2 const & rhs)
{
	x += rhs.x;
	y += rhs.y;
	
	return *this;
}

Rage::Vector2 & Rage::Vector2::operator-=(Rage::Vector2 const & rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	
	return *this;
}

Rage::Vector2 & Rage::Vector2::operator*=(float rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

Rage::Vector2 & Rage::Vector2::operator/=(float rhs)
{
	x /= rhs;
	y /= rhs;

	return *this;
}

Rage::Vector2 Rage::Vector2::GetNormalized() const
{
	float scale = 1.0f / std::sqrt( x * x + y * y );
	return (*this) * scale;
}
