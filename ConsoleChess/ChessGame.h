#pragma once

#include <iostream>
#include <map>

#include "PieceDef.h"
#include "BoardState.h"

class ChessGame
{
public:
	
	std::map<byte, PieceDef> pieceDefs;
	BoardState boardState;

	/// Class constructor (default)
	ChessGame(std::map<byte, PieceDef> pieces) : pieceDefs(pieces), boardState() {};
	//// Constructor (w/state)
	ChessGame(std::map<byte, PieceDef> pieces, BoardState bstate) : pieceDefs(pieces), boardState(bstate) {};
	
	void DrawGrid()
	{
		// Get console STD Handle
		HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
		// Prepare COORD structure 
		COORD curPos;
		curPos.X = 0;
		curPos.Y = 0;

		for (int i = 0; i < 32; i++)
		{
			curPos.Y = i;
			SetConsoleCursorPosition(hCons, curPos);
			for (int j = 0; j < 8; j++)
			{
				// Check parity to figure out color
				SetConsoleTextAttribute(hCons, ((i/4 & 1) == (j & 1)) ? 0x7F : 0x8F);
				std::cout << "       ";
			}
			
		}
	}

	void UpdateSquare(IVec2 pos)
	{
		// Get console STD Handle
		HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
		// Prepare COORD structure 
		COORD curPos;
		curPos.X = 7 * pos.x + 2;
		curPos.Y = 4 * pos.y + 1;

		if (!boardState.PieceAt(pos))
		{
			SetConsoleTextAttribute(hCons, ((pos.x & 1) == (pos.y & 1)) ? 0x7F : 0x8F);

			SetConsoleCursorPosition(hCons, curPos);
			std::cout << "   ";
			curPos.Y++;
			SetConsoleCursorPosition(hCons, curPos);
			std::cout << "   ";
		}
		else
		{
			byte idx = boardState.PieceAt(pos);
			bool team = boardState.TeamAt(pos);

			PieceDef piece = this->pieceDefs.at(idx);

			SetConsoleTextAttribute(hCons, team ? 0xF0 : 0x0F);

			SetConsoleCursorPosition(hCons, curPos);
			std::cout << piece.model[0] << piece.model[1] << piece.model[2];
			curPos.Y++;
			SetConsoleCursorPosition(hCons, curPos);
			std::cout << piece.model[3] << piece.model[4] << piece.model[5];
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
private:

};

