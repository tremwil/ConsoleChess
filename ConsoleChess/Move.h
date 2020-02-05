#pragma once
#include "constants.h"

#define _MF_HIGH_INFO 0x00F0'0000
#define _MF_CAPTURE 0x4000
#define _MF_PROMOTE 0x8000

#define MF_PROMOTE 0b1000
#define MF_CAPTURE 0b0100
#define MF_CASTLE  0b0010
#define MF_PASSANT 0b0001

class Move
{
protected:
	/* move bit data structure:
	*	   |           LOW WORD            |          HIGH WORD            |
	*    i |0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F|
	*    s | fin. pos. | init pos. | flags | cap.| castle|H|  unused byte  |
	*  flags:
	*    bits: 3 (promote), 2 (capture), 1 (sp 1), 0 (sp 0).
	*    Special bits vary based on first 2:
	*    Case 0: No promote, No capture -> 0 = normal, 1 = double pawn, 2 = king side castle, 3 = queen side castle
	*	 Case 1: No promote, capture    -> 0 = normal, 1 = en passant
    *	 Case 2/3: Promote			 	-> 0 = knight, 1 = bishop, 2 = rook, 3 = queen 
	*/

	// Move data (16 bits for low-information and 24 bits for high information).
	U32 move;
public:
	// Create move directly from move code. DO NOT overwrite MF_HIGH_INFO bit.
	Move(U32 moveCode)
	{
		move = moveCode;
	}
	// Create low-information move (not reversible).
	Move(U32 from, U32 to, U32 flags)
	{
		move = (to & 0x3f) | (from & 0x3f) << 6 | (flags & 0x10) << 12;
	}

	// Create high-information move (reversible).
	Move(U32 from, U32 to, U32 flags, Piece captured, U32 prvCastleRights)
	{
		move = (to & 0x3f) | (from & 0x3f) << 0x06 | (flags & 0x10) << 0x0C |
			captured << 0x10 | (prvCastleRights & 0x10) << 0x13 | 0x00F0'0000;
	}

	// Create high-information move from low-info move.
	Move(Move lowInfo, Piece captured, U32 prvCastleRights)
	{
		move = (lowInfo.move & 0xffff) | (prvCastleRights & 0x10) << 0x13 | 0x00F0'0000;
	}

	// Get final position of piece that moved (to).
	U32 finalPos() 
	{
		return move & 0x3f;
	}
	// Get initial posiition of piece that moved (from).
	U32 initPos()
	{
		return (move >> 6) & 0x3f;
	}
	// Get raw move flags.
	U32 getFlags()
	{
		return (move >> 12) & 0x10;
	}
	// Get promoted piece type.
	// UNDOCUMENTED BEHAVIOUR if move is not a promotion.
	Piece getPromotedPiece()
	{
		return (Piece)((getFlags() & 3));
	}

	// Get castle rights before the move.
	// UNDOCUMENTED BEHAVIOUR if move is low-information.
	U32 getPrvCastleRights()
	{
		return (move >> 0x13) & 0x10;
	}
	// Get captured piece type.
	// UNDOCUMENTED BEHAVIOUR if move is not a capture or is low-information.
	Piece getCapturedPiece()
	{
		return (Piece)((move >> 0x10) & 0x8);
	}
	// Return new move containing high-info data.
	Move toHighInfo(Piece captured, U32 prvCastleRights)
	{
		return Move((move & 0xffff) | (prvCastleRights & 0x10) << 0x13 | 0x00F0'0000);
	}

	// Check if move contains additionnal information.
	bool isHighInfo()
	{
		return move & _MF_HIGH_INFO;
	}
	// Check if move is promotion.
	bool isPromote()
	{
		return move & _MF_PROMOTE;
	}
	// Check if move is capture.
	bool isCapture()
	{
		return move & _MF_CAPTURE;
	}
	// Check if move is a castle.
	bool isCastle()
	{
		return !((getFlags() >> 1) ^ 0b001);
	}
	// Check if move is a quiet move (no capture / special).
	bool isQuiet()
	{
		return getFlags() == 0b0000;
	}
	// Check if move is a double pawn push.
	bool isDoublePawn()
	{
		return getFlags() == 0b0001;
	}
	// Check if move is a king side castle.
	bool isKingCastle()
	{
		return getFlags() == 0b0010;
	}
	// Check if move is a queen side castle.
	bool isQueenCastle()
	{
		return getFlags() == 0b0011;
	}

	// Convert move to low-information.
	Move toLowInfo()
	{
		return Move(move & 0xffff);
	}
	// Convert move to U32.
	U32 toU32() 
	{
		return move;
	}
	// Convert move to U16.
	U16 toU16()
	{
		return (U16)move;
	}

	// Assignment operator
	void operator=(Move m)
	{
		move = m.move;
	}
	// Equality and inequality operators
	bool operator==(Move m)
	{
		return (move & 0xffff) == (m.move & 0xffff);
	}
	bool operator!=(Move m)
	{
		return (move & 0xffff) != (m.move & 0xffff);
	}
};