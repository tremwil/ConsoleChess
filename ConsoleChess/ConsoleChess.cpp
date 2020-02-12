// ConsoleChess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <string>

#include "ChessGame.h"
#include "UnitMovePiece.h"
#include "SpriteDefs.h"
#include <vector>

#include "Pawn.h"
#include "King.h"

GameWindow cEx;
bool mouseDown;
UINT64 bb;

static const char* hex = "0123456789ABCDEF";

void onMouse(MOUSE_EVENT_RECORD evt)
{
	IVec2 mP = IVec2(evt.dwMousePosition.X / 8, evt.dwMousePosition.Y / 8);
	int shift = POS_TO_INDEX(mP);
	if (!mouseDown && (evt.dwButtonState & RI_MOUSE_BUTTON_1_DOWN) && shift >= 0 && mP.isChessPos())
	{
		mouseDown = true;

		bb ^= 1ULL << shift;
		cEx.drawSprite((bb & 1ULL << shift) ? 0xff : 0x00, 8*mP.y, 8*mP.x);

		char str[19] { '0', 'x' };
		str[18] = 0;
		UINT64 c = bb;
		for (int i = 15; i >= 0; i--)
		{
			str[i + 2] = hex[c & 0xf];
			c >>= 4;
		}

		//put your text in source
		if (OpenClipboard(NULL))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, 19);
			buffer = (char*)GlobalLock(clipbuffer);
			strcpy_s(buffer, 19, str);
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
	mouseDown = evt.dwButtonState & RI_MOUSE_BUTTON_1_DOWN;
}

int main()
{
	//bb = 0;
	//mouseDown = false;
	//cEx = ConsoleEx();
	//cEx.onMouseEvent = onMouse;
	//cEx.initSpriteMode(14, 13);
	//cEx.setConsoleBufferSize(64, 96);

	//while (true)
	//{
	//	cEx.eventTick();
	//}

	//return 0;

	// Pawn definition
	Pawn pawn = Pawn(1, PawnSprite);
	// Bishop definition
	UnitMovePiece bishop = UnitMovePiece(2, false, false, BishopSprite);
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 1)}, MoveSymmetry::Rotate90, true);
	// Knight definition
	UnitMovePiece knight = UnitMovePiece(3, false, true, KnightSprite);
	knight.generateMoveset(std::vector<IVec2> {IVec2(2, 1)}, MoveSymmetry::Rotate90 | MoveSymmetry::FlipY, false);
	// Rook definition
	UnitMovePiece rook = UnitMovePiece(4, false, false, RookSprite);
	rook.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, MoveSymmetry::Rotate90, true);
	// Queen definition
	UnitMovePiece queen = UnitMovePiece(5, false, false, QueenSprite);
	queen.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, MoveSymmetry::Rotate45, true);
	// King definition
	King king = King(6, 4, KingSprite);

	std::vector<PieceDef*> pieces
	{
		&pawn,
		&bishop,
		&knight,
		&rook,
		&queen,
		&king
	};

	BoardState board = BoardState(new byte[64]
	{
		0x04, 0x03, 0x02, 0x05, 0x06, 0x02, 0x03, 0x04,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x14, 0x13, 0x12, 0x15, 0x16, 0x12, 0x13, 0x14
	});

	ChessGame game = ChessGame(pieces, board);
	game.mainloop();
}