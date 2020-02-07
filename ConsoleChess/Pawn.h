#pragma once
#include "PieceDef.h"

class Pawn : PieceDef
{
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
			if (delta == IVec2(dir, 0)) return true;
			if (delta == IVec2(2*dir,0) && !p.moved && board[start + IVec2(2*dir,0)] == 0) return true;
		}
		// Capture / en passant case
		for (int i = -1; i <= 1; i += 2)
		{
			if (delta == IVec2(dir, i))
			{
				// Normal capture
				Piece cap = board.getPiece(start + IVec2(dir, i));
				if (cap.team != p.team && cap.id != 0) return true;
				// En passant
				cap = board.getPiece(start + IVec2(0, i));
				if (cap.team != p.team && cap.id == id && cap.special == 1) return true;
			}
		}
		return false;
	}

	// Make move for simple pieces, overwrite if necessary (ex. en passant and castle)
	void makeMove(IVec2 start, IVec2 end, BoardState& board) override
	{
		board[end] = board[start] | PIECE_MOVED; // Set piece moved flag
		board[start] = 0;
	}
};