#pragma once
#include "constants.h"
#include "bitboard.h"
#include "Move.h"
#include <stack>

#define CASTLE_WKS 1
#define CASTLE_WQS 2
#define CASTLE_BKS 4
#define CASTLE_BQS 8

class Board
{
public:
	// Net occupancy bitboard (1 where there is a piece, 0 everywhere else).
	U64 occNet;
	
	// Occupancy bitboard.
	U64 occ[15];

	// Square-centric board.
	U32 sqr[64];

	// En passant square (as LESM index).
	U32 enPassant;

	/* Bit structure for castle rights
	*    i |0    |1    |2    |3    |
	*    s |w. ks|w. qs|b. ks|b. qs|
	*/
	// Castle rights.
	U32 castleRights;

	// Team which ought to move on this turn.
	Team sideToMove;

	// Counter for the 50-move rule.
	U32 halfMoveCnt;

	// Queue containing move history of the game.
	std::stack<Move> moves;

	// Init board like normal chess game.
	Board() :
		occNet(0xffff'0000'0000'ffffULL),
		occ
		{
			0,
			0x0010ULL,
			0xff00ULL,
			0x0042ULL,
			0x0024ULL,
			0x0081ULL,
			0x0008ULL,
			0,
			0,
			0x1000'0000'0000'0000ULL,
			0x00ff'0000'0000'0000ULL,
			0x4200'0000'0000'0000ULL,
			0x2400'0000'0000'0000ULL,
			0x8100'0000'0000'0000ULL,
			0x0800'0000'0000'0000ULL
		},
		sqr
		{
			
		},
		enPassant(0),
		castleRights(0x0f),
		sideToMove(Team::white),
		halfMoveCnt(0),
		moves{ } { };

	// Return if there is a piece at given position.
	bool isPieceAt(U32 index)
	{
		return occNet & (1ULL << index);
	}

	// Clear a square on the board from pieces.
	void clearPieceAt(U32 index)
	{
		// Get mask of all but indexed bit
		U64 mask = ~(1ULL << index);
		// Set occupancies and square centric to 0
		occNet &= mask; 
		occ[sqr[index]] &= mask;
		sqr[index] = 0;
	}

	// Set a piece to the board.
	// UNDOCUMENTED BEHAVIOUR when passing a null Piece (0).
	void setPieceAt(U32 index, Piece piece, Team team)
	{
		setPieceAt(index, (TeamPiece)((team << 4) | piece));
	}

	// Set a piece to the board.
	// UNDOCUMENTED BEHAVIOUR when passing a null Piece (0 or 8).
	void setPieceAt(U32 index, TeamPiece piece)
	{
		// Get indexed bit as bitboard along with inverse mask
		U64 sBit = 1ULL << index;
		// Clear previous piece
		occ[sqr[index]] &= ~sBit;
		// set in occupancies and square centric repr.
		occNet |= sBit;
		occ[piece] |= sBit;
		sqr[index] = piece;
	}

	void makeMove(U32 from, U32 to)
	{

	}

	// Make move from Move object.
	void makeMove(Move m)
	{
		U32 from = m.initPos();
		U32 to = m.finalPos();
		U32 flags = m.getFlags();

		if (!m.isHighInfo)
		{	// Make move high info if it is not.
			m = m.toHighInfo(PPIECE(sqr[to]), castleRights);
		}

		// Remove piece from initial square
		occ[sqr[from]] &= ~(1ULL << from);
		occNet &= ~(1ULL << from);
		sqr[from] = 0;

		// Do move
		if (flags & MF_PROMOTE)
		{
			
		}
		// Castling
		else if (flags == 0b101)
		{

		}
		// Set castling flags
		castleRights &= (0b11 << (sideToMove << 1))

		// Handle capture
		if (flags & MF_CAPTURE)
		{
			if ((flags & 7) == 1)
			{
				U32 tgtSqr = 
			}
		}
		// Set en passant flag
		// flags & (~flags | 1) is 0b1111 when flags == 0b0001 and 0 otherwise 
		enPassant = flags & (~flags | 1) & to;

		sideToMove = (Team)(sideToMove ^ 1);
		moves.push(m);
	}

	// Undo last move from stack.
	void unmakeMove()
	{
		unmakeMove(moves.top());
	}

	// Undo last move from Move object.
	void unmakeMove(Move m)
	{
		moves.pop();
	}
};