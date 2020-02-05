#pragma once

typedef unsigned long long U64;
typedef signed long long S64;

typedef unsigned long U32;
typedef signed long S32;

typedef unsigned short U16;
typedef signed short S16;

typedef unsigned char U8;
typedef signed char S8;

// Enum for the different pieces of the chess board
enum Piece : U32
{
	pawn   = 0x1,
	knight = 0x2,
	bishop = 0x3,
	rook   = 0x4,
	queen  = 0x5,
	king   = 0x6
};

// Piece + team as 4th bit
enum TeamPiece : U32
{
	wPawn	= 0x1,
	wKnight = 0x2,
	wBishop = 0x3,
	wQook	= 0x4,
	wQueen	= 0x5,
	wKing   = 0x6,

	bPawn	= 0x9,
	bKnight = 0xA,
	bBishop = 0xB,
	bQook	= 0xC,
	bQueen	= 0xD,
	bKing   = 0xE,
};

// Helper macros for TeamPiece
#define PTEAM(x) (x >> 3)
#define PPIECE(x) ((Piece)(x & 7))

// Enum mapping the two teams to an integer
enum Team : U32
{
	white, black
};