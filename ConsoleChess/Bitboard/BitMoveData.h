#pragma once
#include <Windows.h>
#include "IVec2.h"

struct MoveData
{
	// Using only 15x15, but this improves speed because
	// we can use bitwise ops instead of modulo and mult.
	bool moveTable[256];

	bool isMoveAllowed(IVec2 mv)
	{
		return moveTable[(mv.x + 7) << 8 | (mv.y + 7)];
	}

	void setMoveAllowed(IVec2 mv, bool good)
	{
		moveTable[(mv.x + 7) << 8 | (mv.y + 7)] = good;
	}
};