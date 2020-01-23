#pragma once
#include <Windows.h>
#include "IVec2.h"

#define posToBoard(V) (V.y << 3) | V.x

/// Class representing chess board
class BoardState
{
public:
	// The board data, with pieces stored as numbers in a 8x8 array
	byte board[64];
	// Ctor: init all zeros (empty board)
	BoardState() {};

	// Team (0 = black, 1 = white) at a position
	byte TeamAt(IVec2 pos) 
	{
		return board[posToBoard(pos)] >> 7;
	}
	// Piece (0 = no piece, other = piece ID) at a position
	byte PieceAt(IVec2 pos)
	{
		return board[posToBoard(pos)] & 0x7F;
	}
	// Full data at position (byte where 7 LSB = ID and MSB = team)
	byte DataAt(IVec2 pos)
	{
		return board[posToBoard(pos)];
	}
	// Set Team (0 = black, 1 = white) at a position
	byte SetTeamAt(IVec2 pos, byte team)
	{
		board[posToBoard(pos)] &= (team << 8) | 0x7F;
	}
	// Set Piece (0 = no piece, other = piece ID) at a position
	byte SetPieceAt(IVec2 pos, byte piece)
	{
		board[posToBoard(pos)] &= piece | 0x80;
	}
	// Set Full data at position (byte where 7 LSB = ID and MSB = team)
	byte SetDataAt(IVec2 pos, byte data)
	{
		board[posToBoard(pos)] = data;
	}

	// Move piece from a position to another
	void Move(IVec2 start, IVec2 end)
	{
		int idx = posToBoard(start);
		byte data = board[idx];
		board[idx] = 0;
		board[posToBoard(end)] = data;
	}
};