#pragma once
#include <cmath>
#include <Windows.h>

/// INT8 (signed char) Vec2
struct IVec2
{
	INT8 x; // X coordinate
    INT8 y; // Y corrdinate
 
	IVec2() : x(0), y(0) {};
	IVec2(INT8 x, INT8 y) : x(x), y(y) {};

	// Square of Euclidian distance
	INT8 lenSqr()
	{
		return x * x + y * y;
	}

	// Euclidian (L2) length
	float len()
	{
		return sqrtf((float)(x * x + y * y));
	}

	// L1 metric length (taxicab metric)
	INT8 lenL1() 
	{
		return abs(x) + abs(y);
	}

	// True if a perfect multiple of other vector
	bool multipleOf(const IVec2& v) 
	{
		return x % v.x == 0 && y % v.y == 0;
	}

#pragma region Operators

	IVec2& operator=(const IVec2& a)
	{
		x = a.x;
		y = a.y;
		return *this;
	}

	IVec2 operator+(const IVec2& a)
	{
		return IVec2(x + a.x, y + a.y);
	}

	IVec2& operator+= (const IVec2& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	IVec2 operator-()
	{
		return IVec2(-x, -y);
	}

	IVec2 operator-(const IVec2& a)
	{
		return IVec2(x - a.x, y - a.y);
	}

	IVec2& operator-= (const IVec2& a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	IVec2 operator*(INT8 s)
	{
		return IVec2(x * s, y * s);
	}

	IVec2& operator*= (INT8 s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	IVec2 operator*(const IVec2& a)
	{
		return IVec2(x * a.x, y * a.y);
	}

	IVec2& operator*= (const IVec2& a)
	{
		x *= a.x;
		y *= a.y;
		return *this;
	}

	IVec2 operator/(INT8 s)
	{
		return IVec2(x / s, y / s);
	}

	IVec2& operator/= (INT8 s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	IVec2 operator/(const IVec2& a)
	{
		return IVec2(x / a.x, y / a.y);
	}

	IVec2& operator/= (const IVec2& a)
	{
		x /= a.x;
		y /= a.y;
		return *this;
	}

	IVec2 operator%(const IVec2& a)
	{
		return IVec2(x % a.x, y % a.y);
	}

	IVec2& operator%= (const IVec2& a)
	{
		x %= a.x;
		y %= a.y;
		return *this;
	}

	bool operator== (const IVec2& a)
	{
		return a.x == x && a.y == y;
	}

	bool operator!= (const IVec2& a)
	{
		return a.x != x || a.y != y;
	}
#pragma endregion
};