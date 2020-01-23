// ConsoleChess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <string>

#include "ChessGame.h"
#include "UnitMovePiece.h"
#include <map>

int main()
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 12;                   // Width of each character in the font
	cfi.dwFontSize.Y = 20;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Courrier New");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    std::cout << "Hello World!\n";

	// Bishop definition
	UnitMovePiece bishop = UnitMovePiece(2, 1, false, " G /_\\");
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 1)}, MoveSymmetry::Rotate90, true);
	// Knight definition
	UnitMovePiece knight = UnitMovePiece(3, 1, true, "{@\\}_\\");
	bishop.generateMoveset(std::vector<IVec2> {IVec2(2, 1)}, MoveSymmetry::Rotate90 | MoveSymmetry::FlipY, false);
	// Rook definition
	UnitMovePiece rook = UnitMovePiece(4, 1, false, "\xF1\xF1\xF1[_]");
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, MoveSymmetry::Rotate90, true);
	// Queen definition
	UnitMovePiece queen = UnitMovePiece(5, 1, false, "WWW]_[");
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, MoveSymmetry::Rotate45, true);
	// King definition
	UnitMovePiece king = UnitMovePiece(6, 0, false, "O+O]_[");
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, MoveSymmetry::Rotate45, false);


	std::map<byte, PieceDef> pieces
	{
		{2, bishop},
		{3, knight},
		{4, rook},
		{5, queen},
		{6, king}
	};

	BoardState board = BoardState();
	memcpy_s(board.board, 64, new byte[64]
	{
		0x04, 0x03, 0x02, 0x05, 0x06, 0x02, 0x03, 0x04,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x84, 0x83, 0x82, 0x85, 0x86, 0x82, 0x83, 0x84
	}, 64);

	ChessGame game = ChessGame(pieces, board);
	game.DrawGrid();
	game.UpdateAll();
}