#pragma once

#include <iostream>
#include <Windows.h>
#include <map>
#include <vector>

#include "ConsoleEx.h"
#include "PieceDef.h"
#include "BoardState.h"

typedef enum 
{
	FullBlack		= 0x0,
	WhiteFill		= 0x1,
	WhiteOutline	= 0x2,
	BlackFill		= 0x3,
	BlackOutline	= 0x4,
	SquareWhite		= 0x5,
	SquareBlack		= 0x6,
	SquareInCheck	= 0x7,
	SquareSelected	= 0x8,
	SquarePossMove	= 0x9,
	TextFg			= 0xA,
	TextBg			= 0xB
} ChessColor;

class ChessGame
{
private:
	ConsoleEx consoleEx;

	IVec2 selectedSquare;
	Byte88 possibleMoves;

	Byte88 sqrHighlightSprite;

	void init(std::vector<PieceDef> pieces)
	{
		pieceDefs = std::vector<PieceDef>{ PieceDef() };
		pieceDefs.insert(pieceDefs.end(), pieces.begin(), pieces.end());
		for (int i = 1; i < pieceDefs.size(); i++)
		{
			pieceDefs[i].id = i;
		}

		consoleEx = ConsoleEx();
		consoleEx.initSpriteMode(14, 13);
		consoleEx.setConsoleSize(15 * 8, 17 * 8);

		consoleEx.colormap[FullBlack] = 0x000000;
		consoleEx.colormap[WhiteFill] = 0xe8f1ff;
		consoleEx.colormap[WhiteOutline] = 0xc7c3c2;
		consoleEx.colormap[BlackFill] = 0x9c7683;
		consoleEx.colormap[BlackOutline] = 0x532b1d;
		consoleEx.colormap[SquareWhite] = 0xffccaa;
		consoleEx.colormap[SquareBlack] = 0xab5632;
		consoleEx.colormap[SquareInCheck] = 0x0335fc;
		consoleEx.colormap[SquareSelected] = 0x90f5b7;
		consoleEx.colormap[SquarePossMove] = 0x00d138;
		consoleEx.colormap[TextBg] = 0x404040;
		consoleEx.colormap[TextFg] = 0xd0d0d0;
		consoleEx.applyColormap();
	}
public:
	
	std::vector<PieceDef> pieceDefs;
	Byte88 boardState;

	/// Class constructor (default)
	ChessGame(std::vector<PieceDef> pieces) : boardState(), possibleMoves(), selectedSquare(-1, -1)
	{
		init(pieces);
	};
	//// Constructor (w/state)
	ChessGame(std::vector<PieceDef> pieces, Byte88 bstate) : boardState(bstate), possibleMoves(), selectedSquare(-1,-1)
	{
		init(pieces);
	};
	
	void UpdateSquare(IVec2 pos)
	{
		int piece = boardState[pos];
		// Draw square
		bool isWhiteSqr = (pos.x & 1) == (pos.y & 1);
		Byte88 sqrSprite = Byte88((isWhiteSqr ? SquareWhite : SquareBlack) << 4);
		consoleEx.drawSprite(sqrSprite, 8 * pos.y, 8 * pos.x);
		// Draw board marks 
		if (possibleMoves[pos] || pos == selectedSquare) 
		{
			sqrSprite = Byte88((possibleMoves[pos] ? SquarePossMove : SquareSelected) << 4);
			consoleEx.drawSpriteAlpha(sqrSprite, 8 * pos.y, 8 * pos.x);
		}
		// Draw piece
		if (piece != 0)
		{
			Byte88 sprite = Byte88(pieceDefs[piece & 0x7f].sprite);
			if (piece & 0x80) { sprite = (sprite >> 4) & 0xf0; }
			consoleEx.drawSpriteAlpha(sqrSprite, 8 * pos.y, 8 * pos.x);
		}
	}

	void UpdateAll()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				UpdateSquare(IVec2(i, j));
			}
		}
	}
};

