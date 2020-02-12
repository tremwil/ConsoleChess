#pragma once
#include <cmath>
#include <Windows.h>

/// int (signed char) Vec2
struct IVec2
{
	int x; // X coordinate
    int y; // Y corrdinate
 
	IVec2() : x(0), y(0) {};
	IVec2(int x, int y) : x(x), y(y) {};

	// Square of Euclidian distance
	int lenSqr() const
	{
		return x * x + y * y;
	}

	// Euclidian (L2) length
	float len() const
	{
		return sqrtf((float)(x * x + y * y));
	}

	// L1 metric length (taxicab metric)
	int lenL1() const
	{
		return abs(x) + abs(y);
	}

	// True if a perfect multiple of other vector
	bool multipleOf(const IVec2& v) const
	{
		return x % v.x == 0 && y % v.y == 0;
	}

	// True if this Vec2 represents a position on a chess board 
	bool isChessPos() const
	{
		return 0 <= x && x < 8 && 0 <= y && y < 8;
	}

#pragma region Operators

	IVec2& operator=(const IVec2& a)
	{
		x = a.x;
		y = a.y;
		return *this;
	}

	IVec2 operator+(const IVec2& a) const
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

	IVec2 operator-(const IVec2& a) const
	{
		return IVec2(x - a.x, y - a.y);
	}

	IVec2& operator-= (const IVec2& a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	IVec2 operator*(int s) const
	{
		return IVec2(x * s, y * s);
	}

	IVec2& operator*= (int s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	IVec2 operator*(const IVec2& a) const
	{
		return IVec2(x * a.x, y * a.y);
	}

	IVec2& operator*= (const IVec2& a)
	{
		x *= a.x;
		y *= a.y;
		return *this;
	}

	IVec2 operator/(int s) const
	{
		return IVec2(x / s, y / s);
	}

	IVec2& operator/= (int s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	IVec2 operator/(const IVec2& a) const
	{
		return IVec2(x / a.x, y / a.y);
	}

	IVec2& operator/= (const IVec2& a)
	{
		x /= a.x;
		y /= a.y;
		return *this;
	}

	IVec2 operator%(const IVec2& a) const
	{
		return IVec2(x % a.x, y % a.y);
	}

	IVec2& operator%= (const IVec2& a)
	{
		x %= a.x;
		y %= a.y;
		return *this;
	}

	bool operator== (const IVec2& a) const
	{
		return a.x == x && a.y == y;
	}

	bool operator!= (const IVec2& a) const
	{
		return a.x != x || a.y != y;
	}
#pragma endregion
};

IVec2 operator*(int s, const IVec2& a)
{
	return a.operator*(s);
}

IVec2 operator/(int s, const IVec2& a)
{
	return a.operator/(s);
}