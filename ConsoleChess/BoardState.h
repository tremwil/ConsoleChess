#pragma once
#include <Windows.h>
#include "IVec2.h"
#include "Byte88.h"

#define PIECE_ID      0b00001111
#define PIECE_TEAM    0b00010000
#define PIECE_MOVED   0b00100000
#define PIECE_SP1     0b01000000
#define PIECE_SP2	  0b10000000
#define PIECE_SPECIAL 0b11000000

struct Piece
{
	byte id : 4;		// Piece ID (1-15)
	byte team : 1;		// Piece team (black/white)
	byte moved : 1;		// if piece moved since start
	byte special : 2;	// special bits reserved for the piece

	Piece(byte b)
	{
		id = b & 0xf; b >>= 4;
		team = b & 1; b >>= 1;
		moved = b & 1; b >>= 1;
		special = b;
	}

	byte getByte()
	{
		return special << 6 | moved << 5 | team << 4 | id;
	}
};

/// Byte88 extension to represent chess board
struct BoardState : Byte88
{
	// Create empty byte8x8.
	BoardState() : Byte88() {};

	// Create copy of byte8x8.
	BoardState(const BoardState& b)
	{
		memcpy_s(data, 64, b.data, 64);
	}

	BoardState(byte b)
	{
		std::fill_n(data, 64, b);
	}

	// Create a BoardState from a bit board with custom LOW and HIGH bytes. 
	BoardState(UINT64 bboard, byte low, byte high) : Byte88()
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] = (bboard & 1) ? high : low;
			bboard >>= 1;
		}
	}

	// Create a byte8x8 from a pointer. Unsafe.
	BoardState(byte* ptr)
	{
		memcpy_s(data, 64, ptr, 64);
	}

	Piece getPiece(int pos) const
	{
		return Piece(data[pos]);
	}

	Piece getPiece(IVec2 pos) const
	{
		return Piece(data[pos.y << 3 | pos.x]);
	}

	void setPiece(int pos, Piece p)
	{
		data[pos] = p.getByte();
	}

	void setPiece(IVec2 pos, Piece p)
	{
		data[pos.y << 3 | pos.x] = p.getByte();
	}
};