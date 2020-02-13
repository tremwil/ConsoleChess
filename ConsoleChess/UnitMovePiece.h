#pragma once
#include <Windows.h>
#include <vector>
#include "PieceDef.h"
#include <algorithm>

// Enum defining the types of symmetry that can be applied to unit moves. 
enum MoveSymmetry : int
{
	// First 2 bits determines reflection (ex. knight) - APPLIED LAST, 
	// Thrid 45deg. rotation (ex. queen),
	// Fourth 90deg. rotation (ex. rook)
	None =		0b0000, 
	Rotate90 =	0b0001,
	Rot90_45 =  0b0010,
	Rotate45 =	0b0011,
	FlipX =		0b0100,
	FlipY =		0b1000
};

// Type of PieceDef with moves generated from unit moves 
class UnitMovePiece : public PieceDef
{
private:
	// 16x16 byte array storing moveset information.
	byte moveset[256];


public:
	// If true, this piece can jump over other pieces.
	bool canJump;

	// Ctor
	UnitMovePiece(byte id, bool critical, bool canJump, Byte88 sprite) : moveset{ }
	{
		this->id = id;
		this->critical = critical;
		this->canJump = canJump;
		this->sprite = Byte88(sprite);
	}

	// Initialize moveset data based on given unit moves with symmetry, etc.
	void generateMoveset(std::vector<IVec2> unitMoves, int sym, bool repeat)
	{
		// First fill apply symmetries on the unit moves.
		if (sym & Rotate90) // 90 deg. rotate
		{
			int csz = unitMoves.size();
			for (int i = 0; i < csz; i++) 
			{
				IVec2 mv = unitMoves[i]; // Orig. move
				IVec2 dv = IVec2(mv.x - mv.y, mv.x + mv.y); // 45deg. rotated move
				// Rotate orig. by 90deg. on other axes
				unitMoves.push_back(IVec2(-mv.y, mv.x));
				unitMoves.push_back(IVec2(mv.y, -mv.x));
				unitMoves.push_back(IVec2(-mv.x, -mv.y));
				// 45 deg rotate
				if (sym & Rot90_45) 
				{	// Add 90deg. rotations of the 45deg. rotation
					unitMoves.push_back(dv);
					unitMoves.push_back(IVec2(-dv.y, dv.x));
					unitMoves.push_back(IVec2(dv.y, -dv.x));
					unitMoves.push_back(IVec2(-dv.x,-dv.y));
				}
			}
		}
		if (sym & (FlipX | FlipY)) // Reflect
		{
			int csz = unitMoves.size();
			for (int i = 0; i < csz; i++)
			{
				IVec2 mv = unitMoves[i]; // Orig. move
				if (sym & FlipX) { unitMoves.push_back(IVec2(-mv.x, mv.y)); } // X Flip
				if (sym & FlipY) { unitMoves.push_back(IVec2(mv.x, -mv.y)); } // Y Flip
			}
		}
		// Fill moveset data from generated unit moves

		// Set moveset to zero and will moveset table
		std::fill_n(moveset, sizeof(moveset), 0);
		for (int i = 0; i < unitMoves.size(); i++) 
		{
			IVec2 mv = unitMoves[i]; // Current unit move
			IVec2 delta = mv; // Current delta
			int j = 0x88; // moveset index for 0 delta

			do 	// While move is in range of the board, repeat it
			{
				// Calculate current moveset index and set to previous
				int jc = (delta.y + 8) << 4 | (delta.x + 8);
				moveset[jc] = j;
				// Update moveset index and increment delta
				j = jc;
				delta += mv;
			} while (repeat && abs(delta.x) < 8 && abs(delta.y) < 8);
		}
	}

	// Check if potential move is pseudolegal
	bool isValidMove(IVec2 start, IVec2 end, const BoardState &board) override
	{
		// Check for team of piece at the end 
		if (board[end] != 0 && board.getPiece(end).team == board.getPiece(start).team) { return false; }
		// Find displacement of piece
		IVec2 delta = end - start;
		// Get index in moveset array
		int i = (delta.y + 8) << 4 | (delta.x + 8);
		// Location not in moveset or onto our piece
		if (moveset[i] == 0) { return false; }
		// Location in moveset and can jump over pieces
		else if (canJump) { return true; }
		// Check for possible obstruction
		else
		{
			// Jump to previous positions on the move path until the 0 delta
			while (moveset[i] != 0x88)
			{
				i = moveset[i]; // Get previous position in move path
				IVec2 d = IVec2((i & 0xF) - 8, (i >> 4) - 8); // Calculate equiv. delta
				// Check if piece lies in the intermediate square
				if (board[start + d] != 0) { return false; }
			}
			// Path was not blocked by another piece, move is valid
			return true;
		}
	}
};

