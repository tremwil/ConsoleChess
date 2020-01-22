#pragma once
#include "PieceDef.h"

class ChessGame
{
public:
	
	PieceDef pieceDefs[];

	/// Class constructor (default)
	ChessGame();
	//// Constructor (w/state)
	ChessGame();
	/// Move piece
	Move();
	/// Set board state
	SetState();
private:

};

