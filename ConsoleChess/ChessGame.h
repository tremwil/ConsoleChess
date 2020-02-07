#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>

#include "ConsoleEx.h"
#include "PieceDef.h"
#include "BoardState.h"

static const Byte88 TgtSqrSprite = Byte88(new byte[64]
{
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
});

typedef enum 
{
	FullBlack		= 0x0,
	WhiteFill		= 0x1,
	WhiteOutline	= 0x2,
	BlackFill		= 0x3,
	BlackOutline	= 0x4,
	SquareWhite		= 0x5,
	SquareBlack		= 0x6,
	SquareSelected	= 0x7,
	SquarePossMove	= 0x8,
	SquareInCheck	= 0x9,
	SquareHover		= 0xA,
	TextFg			= 0xB,
	TextBg			= 0xC
} ChessColor;

class ChessGame
{
private:
	ConsoleEx consoleEx;

	IVec2 hoverSqr;
	IVec2 selectedSqr;
	Byte88 attackedCrits;
	Byte88 legalMoves[64];
	BoardState prvBoard;

	void init(std::vector<PieceDef*> pieces)
	{
		pieceDefs = std::vector<PieceDef*>{ NULL };
		pieceDefs.insert(pieceDefs.end(), pieces.begin(), pieces.end());
		for (int i = 1; i < pieceDefs.size(); i++)
		{
			pieceDefs[i]->id = i;
		}

		currTeam = 1;
		hoverSqr = IVec2(-1, -1);
		selectedSqr = IVec2(-1, -1);
		attackedCrits = Byte88();
		prvBoard = BoardState();

		calculateLegalMoves(currTeam);

		consoleEx = ConsoleEx();
		consoleEx.onKeyEvent = [this](KEY_EVENT_RECORD evt) { onKey(evt); };
		consoleEx.onMouseEvent = [this](MOUSE_EVENT_RECORD evt) { onMouse(evt); };
		//consoleEx.onBufferEvent = [this](WINDOW_BUFFER_SIZE_RECORD evt) { onBufferResize(evt); };

		consoleEx.initSpriteMode(14, 13);
		consoleEx.setConsoleBufferSize(64, 64);

		consoleEx.colormap[FullBlack] = 0x000000;
		consoleEx.colormap[WhiteFill] = 0xe8f1ff;
		consoleEx.colormap[WhiteOutline] = 0xc7c3c2;
		consoleEx.colormap[BlackFill] = 0x9c7683;
		consoleEx.colormap[BlackOutline] = 0x532b1d;
		consoleEx.colormap[SquareWhite] = 0xaaccff;
		consoleEx.colormap[SquareBlack] = 0x3256ab;
		consoleEx.colormap[SquareInCheck] = 0x0335fc;
		consoleEx.colormap[SquareSelected] = 0x5bcf88;
		consoleEx.colormap[SquareHover] = 0x90f5b7;
		consoleEx.colormap[SquarePossMove] = 0x90f5b7;
		consoleEx.colormap[TextBg] = 0x404040;
		consoleEx.colormap[TextFg] = 0xd0d0d0;
		consoleEx.applyColormap();
	}
public:
	
	std::vector<PieceDef*> pieceDefs;
	BoardState board;

	byte currTeam;

	// Class constructor (default)
	ChessGame(std::vector<PieceDef*> pieces) : board()
	{
		init(pieces);
	};
	// Constructor (w/state)
	ChessGame(std::vector<PieceDef*> pieces, BoardState bstate) : board(bstate)
	{
		init(pieces);
	};

	// Check if piece is attacked using a brute force approach
	bool isAttacked(IVec2 pos)
	{
		Piece tgt = board.getPiece(pos);
		if (tgt.id == 0) { return false; }
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				Piece att = board.getPiece(v);
				if (att.id != 0 && att.team != tgt.team && pieceDefs[att.id]->isValidMove(v, pos, board))
				{
					return true;
				}
			}
		}
		return false;
	}

	// Make a move (without updating the rendered chess board).
	void makeMove(IVec2 start, IVec2 end)
	{
		// Push copy of current board state
		prvBoard = BoardState(board);
		// Let piece perform the move
		Piece p = board.getPiece(start);
		pieceDefs[p.id]->makeMove(start, end, board);
		// Change current playing team
		currTeam ^= 1;
	}

	// Undo a move (without updating the rendered chess board).
	void undoMove()
	{
		// Copy current board and revert to prv. position
		BoardState temp = BoardState(board);
		board = prvBoard;
		prvBoard = temp;
		// Change current playing team
		currTeam ^= 1;
	}

	// Return true if any critical pieces are under attack
	bool inCheck(bool team)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				Piece p = board.getPiece(v);
				if (p.id == 0 || p.team != team) { continue; }
				if (pieceDefs[p.id]->critical && isAttacked(v)) { return true; }
			}
		}
		return false;
	}

	// Compute critical pieces in check inside attackedCrits and return # of checks
	int computeChecks(bool team)
	{
		attackedCrits = Byte88();
		int cnt = 0;

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				Piece p = board.getPiece(v);
				if (p.id == 0 || p.team != team) { continue; }
				if (pieceDefs[p.id]->critical && isAttacked(v)) 
				{ 
					attackedCrits[v] = 1; 
					cnt++;
				}
			}
		}
		return cnt;
	}

	// Calculate the legal moves of the current team, and return the amount
	// Very inefficient, but C++ is so fast it does not matter
	int calculateLegalMoves(bool team)
	{
		int cnt = 0;
		// iterate through all squares
		for (int k = 0; k < 64; k++)
		{
			// Check if piece is of the curr. team
			IVec2 v = IVec2(k & 7, k >> 3);
			Piece p = board.getPiece(k);
			legalMoves[k] = Byte88();
			if (p.team != team || p.id == 0) continue; 
			// Go through all squares for possible moves
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					IVec2 u = IVec2(i, j);
					// If move is pseudolegal, perform it and check for check
					if (pieceDefs[p.id]->isValidMove(v, u, board))
					{
						makeMove(v, u);
						if (!inCheck(team))
						{
							legalMoves[k][u] = 1;
							cnt++;
						}
						undoMove();
					}
				}
			}
		}
		return cnt;
	}
	
	// Render a single square of the chess board.
	void drawSquare(IVec2 pos)
	{
		Piece piece = board.getPiece(pos);
		Byte88 sprite;
		// Draw board marks if they appear at this square
		if (pos == hoverSqr || pos == selectedSqr)
		{
			sprite = Byte88(((pos == selectedSqr) ? SquareSelected : SquareHover) << 4);
			consoleEx.drawSprite(sprite, 8 * pos.y, 8 * pos.x);
		}
		else
		{
			// Draw square
			bool isWhiteSqr = (pos.x & 1) == (pos.y & 1);
			Byte88 sprite = Byte88((isWhiteSqr ? SquareWhite : SquareBlack) << 4);
			consoleEx.drawSprite(sprite, 8 * pos.y, 8 * pos.x);
		}
		if (selectedSqr.x != -1 && legalMoves[selectedSqr.y << 3 | selectedSqr.x][pos])
		{
			sprite = TgtSqrSprite & 0xe0;
			consoleEx.drawSpriteAlpha(sprite, 8 * pos.y, 8 * pos.x);
		}
		if (attackedCrits[pos])
		{
			consoleEx.drawSpriteAlpha(TgtSqrSprite, 8 * pos.y, 8 * pos.x);
		}
		// Draw piece
		if (piece.id != 0)
		{
			sprite = Byte88(pieceDefs[piece.id]->sprite);
			if (piece.team) { sprite = (sprite >> 1) & 0xf0; }
			consoleEx.drawSpriteAlpha(sprite, 8 * pos.y, 8 * pos.x);
		}
	}

	// Render all squares of the chess board.
	void drawAllBoard()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				drawSquare(IVec2(i, j));
			}
		}
	}

	// Redraw elements of the board that were changed.
	void renderBoardChanges()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				if (prvBoard[v] != board[v]) { drawSquare(v); }
			}
		}
	}

	// Redraw the squares where there is a 1 in the Byte88
	void drawMaskedBoard(const Byte88 &mask)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				if (mask[v]) { drawSquare(v); }
			}
		}
	}

	// Begin the chess game.
	void mainloop()
	{
		drawAllBoard();

		while (true)
		{
			consoleEx.eventTick();
		}
	}

	// Event handler, called during a key press.
	void onKey(KEY_EVENT_RECORD evt)
	{
		if (evt.wVirtualKeyCode == 'Q')
		{
			exit(0);
		}
	}

	// Event handler, called during a mouse event.
	void onMouse(MOUSE_EVENT_RECORD evt)
	{
		IVec2 boardPos = IVec2(evt.dwMousePosition.X / 8, evt.dwMousePosition.Y / 8);
		IVec2 prvSqr = IVec2(-1,-1);

		if (evt.dwButtonState & RI_MOUSE_BUTTON_1_DOWN)
		{
			if (boardPos.isChessPos() && selectedSqr != boardPos)
			{
				if (board[boardPos] == 0 || board.getPiece(boardPos).team != currTeam)
				{
					if (selectedSqr.isChessPos() && legalMoves[selectedSqr.y << 3 | selectedSqr.x][boardPos])
					{
						prvSqr = selectedSqr;
						selectedSqr = IVec2(-1, -1);
						drawMaskedBoard(legalMoves[prvSqr.y << 3 | prvSqr.x]);

						makeMove(prvSqr, boardPos);
						renderBoardChanges();

						int nLegalMoves = calculateLegalMoves(currTeam);
						int nChecks = computeChecks(currTeam);
						drawMaskedBoard(attackedCrits);
						// Check for game state
						if (nLegalMoves == 0)
						{
							if (nChecks == 0) { } // Draw
							else { } // Win
						}
					}
					
				}
				else
				{
					prvSqr = selectedSqr;
					selectedSqr = boardPos;
					drawSquare(selectedSqr);
					drawMaskedBoard(legalMoves[selectedSqr.y << 3 | selectedSqr.x]);
					drawMaskedBoard(legalMoves[prvSqr.y << 3 | prvSqr.x]);
				}
			}
		}
		//if (evt.dwButtonState & RI_MOUSE_BUTTON_2_DOWN)
		//{
		//	prvSqr = selectedSqr;
		//	selectedSqr = IVec2(-1, -1);
		//	if (prvSqr.isChessPos()) drawMaskedBoard(legalMoves[prvSqr.y << 3 | prvSqr.x]);
		//}
		if (evt.dwEventFlags & MOUSE_MOVED)
		{
			if (boardPos.isChessPos() && hoverSqr != boardPos)
			{
				prvSqr = hoverSqr;
				hoverSqr = boardPos;
				drawSquare(boardPos);
			}
		}
		if (prvSqr.isChessPos()) drawSquare(prvSqr);
	}
};

