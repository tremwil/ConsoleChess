#pragma once
#include "PieceDef.h"
#include "SpriteDefs.h"

class Pawn : public PieceDef
{
public:
	// Default pawn ctor
	Pawn(byte id) : PieceDef(id, false, PawnSprite) { }

	// Check if potential move is pseudolegal, implemented by specific piece class
	bool isValidMove(IVec2 start, IVec2 end, BoardState& board) override
	{
		IVec2 delta = end - start;
		Piece p = board.getPiece(start);
		int dir = p.team ? -1 : 1;
		// Blocked at end of board (should promote before this happens)
		if (start.y == (p.team ? 0 : 7)) return false;
		// No captrue case (normal and push move)
		if (board[end] == 0)
		{
			if (delta == IVec2(0,dir)) return true;
			if (delta == IVec2(0,2*dir) && !p.moved && board[start + IVec2(0,dir)] == 0) return true;
		}
		// Capture / en passant case
		for (int i = -1; i <= 1; i += 2)
		{
			if (delta == IVec2(i,dir))
			{
				// Normal capture
				Piece cap = board.getPiece(start + IVec2(i,dir));
				if (cap.team != p.team && cap.id != 0) return true;
				// En passant
				Piece enp = board.getPiece(start + IVec2(i,0));
				if (cap.id == 0 && enp.team != p.team && enp.id == id && enp.special == 1) return true;
			}
		}
		return false;
	}

	// Make move for simple pieces, overwrite if necessary (ex. en passant and castle)
	void makeMove(IVec2 start, IVec2 end, BoardState& board) override
	{
		IVec2 delta = end - start;
		Piece p = board.getPiece(start);

		if (delta.x != 0 && board[end] == 0) // En passant capture
		{
			board[start + IVec2(delta.x, 0)] = 0;
		}
		// Set piece moved flag, and clear special bits
		board[end] = (board[start] | PIECE_MOVED) & ~PIECE_SPECIAL;
		board[start] = 0;
		// Set special bits to 1 if pawn push
		if (abs(delta.y) > 1) board[end] |= PIECE_SP1;
	}
};