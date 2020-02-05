#pragma once

#include <vector>
#include <Windows.h>
#include "IVec2.h"
#include "BoardState.h"
#include "Byte88.h"

class PieceDef
{
public:
	// ID of piece, used to differentiate it from others
	byte id;
	// How important the piece is, 0 = King (cannot be captured)
	byte value;
	// Byte88 struct containing icon for the piece (64 color bytes)
	Byte88 sprite;

	// Constructor
	PieceDef() : id(0), value(0), sprite() {};
	PieceDef(byte id, byte value, Byte88 sprite) : id(id), value(value), sprite(sprite) {};

	// Check if potential move is pseudolegal
	bool IsValidMove(IVec2 start, IVec2 end, bool team, BoardState& board);
	// Return all possible moves for a piece without asserting king safety
	std::vector<IVec2> pseudolegalMoves(IVec2 pos, bool team, BoardState& board);
};