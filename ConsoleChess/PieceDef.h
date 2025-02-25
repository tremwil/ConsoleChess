#pragma once

#include <vector>
#include <Windows.h>
#include "IVec2.h"
#include "BoardState.h"
#include "Byte88.h"

// Absract class for piece definitions. Should be inherited by the pieces to be added
// in the game.
class PieceDef
{
public:
	// ID of piece, used to differentiate it from others
	byte id;
	// True if the piece cannot be captured (King-like)
	bool critical;
	// Byte88 struct containing icon for the piece (64 color bytes)
	Byte88 sprite;

	// Constructor
	PieceDef() : id(0), critical(0), sprite() {};
	PieceDef(byte id, bool critical, Byte88 sprite) : id(id), critical(critical), sprite(sprite) {};

	// Check if potential move is pseudolegal, implemented by specific piece class.
	// The return value is true if the move is valid and false otherwise.
	virtual bool isValidMove(IVec2 start, IVec2 end, const BoardState &board)
	{
		return false;
	}

	// Make move for simple pieces, overwrite if necessary (ex. en passant and castle).
	// The return value is a flag that if true tells the chess game to open a promotion
	// dialog for this piece. In normal chess, only the pawns should return true after they
	// have arrived to the last square.
	virtual bool makeMove(IVec2 start, IVec2 end, BoardState& board)
	{
		board[end] = board[start] | PIECE_MOVED; // Set piece moved flag
		board[start] = 0;
		// Return value controls piece promotion
		return false;
	}
};