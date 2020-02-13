#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>

#include "GameWindow.h"
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

enum ChessColor
{
	Transparent = 0x0,
	WhiteFill = 0x1,
	WhiteOutline = 0x2,
	BlackFill = 0x3,
	BlackOutline = 0x4,
	SquareWhite = 0x5,
	SquareBlack = 0x6,
	SquareSelected = 0x7,
	SquarePossMove = 0x8,
	SquareInCheck = 0x9,
	SquareHover = 0xA,
	FullWhite = 0xB
};

enum Layers
{
	LayerBoard = 0,
	LayerMoves = 1,
	LayerSelected = 2,
	LayerCheck = 3,
	LayerPiece = 4,
	LayerText = 5,
	LayerRestart = 6
};

enum GameState
{
	InProgress,
	Checkmate,
	Stalemate,
	Promoting
};

class ChessGame
{
private:
	GameWindow window;

	IVec2 hoverSqr;
	IVec2 selectedSqr;
	Byte88 attackedCrits;
	Byte88 legalMoves[64];
	BoardState prvBoard;

	int gameState;

	void init(std::vector<PieceDef*> pieces)
	{
		for (int i = 0; i < pieces.size(); i++)
		{
			pieceDefs[pieces[i]->id] = pieces[i];
		}

		window = GameWindow();
		window.onKeyEvent = [this](KEY_EVENT_RECORD evt) { onKey(evt); };
		window.onMouseEvent = [this](MOUSE_EVENT_RECORD evt) { onMouse(evt); };

		window.colormap[Transparent] = 0x000000;
		window.colormap[WhiteFill] = 0xe8f1ff;
		window.colormap[WhiteOutline] = 0xc7c3c2;
		window.colormap[BlackFill] = 0x9c7683;
		window.colormap[BlackOutline] = 0x532b1d;
		window.colormap[SquareWhite] = 0xaaccff;
		window.colormap[SquareBlack] = 0x3256ab;
		window.colormap[SquareInCheck] = 0x0335fc;
		window.colormap[SquareSelected] = 0x5bcf88;
		window.colormap[SquareHover] = 0x90f5b7;
		window.colormap[SquarePossMove] = 0x90f5b7;
		window.colormap[FullWhite] = 0xffffff;
		window.applyColormap();

		window.textFill = WhiteFill << 4;
		window.textOutline = WhiteOutline << 4;
		window.textBackground = 0;
		window.alphaColor = Transparent << 4;

		window.setup(128, 64, 14, 13);

		// Setup layers: board
		window.layers.push_back(Layer(64, 64, Transparent << 4));
		window.layers[LayerBoard].transform([](byte v, IVec2 pos)
		{
			bool isWhite = (pos.x / 8 & 1) == (pos.y / 8 & 1);
			return (isWhite ? SquareWhite : SquareBlack) << 4;
		});
		// Init empty board layers
		window.layers.push_back(Layer(64, 64, Transparent << 4));
		window.layers.push_back(Layer(64, 64, Transparent << 4));
		window.layers.push_back(Layer(64, 64, Transparent << 4));
		window.layers.push_back(Layer(64, 64, Transparent << 4));
		window.layers.push_back(Layer(64, 64, Transparent << 4, IVec2(64, 0)));
		// Init text layers: dynamic and constant for restart btn
		window.layers.push_back(Layer(64, 64, Transparent << 4, IVec2(64, 0)));
		window.spriteText("RESTART", LayerRestart, IVec2(4, 52), WhiteFill << 4, BlackFill << 4, BlackOutline << 4);
	}
public:

	PieceDef* pieceDefs[16];
	BoardState board;
	BoardState startingBoard;

	byte currTeam;

	// Class constructor (default)
	ChessGame(std::vector<PieceDef*> pieces) : startingBoard(), pieceDefs{ }
	{
		init(pieces);
	};
	// Constructor (w/state)
	ChessGame(std::vector<PieceDef*> pieces, BoardState bstate) : startingBoard(bstate), pieceDefs{ }
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
	bool makeMove(IVec2 start, IVec2 end)
	{
		// Push copy of current board state
		prvBoard = BoardState(board);
		// Clear temp special bit
		board &= ~PIECE_SPTEMP;
		// Let piece perform the move
		Piece p = board.getPiece(start);
		bool promote = pieceDefs[p.id]->makeMove(start, end, board);
		// Change current playing team
		currTeam ^= 1;
		// Return promotion flag
		return promote;
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

	void promoteGUI()
	{

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

	void redraw()
	{
		// Draw selecteds
		window.layers[LayerSelected].transform([this](byte v, IVec2 pos)
		{
			pos /= 8;
			return ((pos == selectedSqr)? SquareSelected : (pos == hoverSqr) ? SquareHover : Transparent) << 4;
		});
		window.layers[LayerMoves].setAll(Transparent << 4);
		window.layers[LayerCheck].setAll(Transparent << 4);
		window.layers[LayerPiece].setAll(Transparent << 4);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 pos = IVec2(i, j);
				Piece piece = board.getPiece(pos);
				Byte88 sprite;

				if (selectedSqr.x != -1 && legalMoves[selectedSqr.y << 3 | selectedSqr.x][pos])
				{
					sprite = TgtSqrSprite & 0xe0;
					window.layers[LayerMoves].drawSprite(sprite, 8 * pos, Transparent << 4);
				}
				if (attackedCrits[pos])
				{
					window.layers[LayerCheck].drawSprite(TgtSqrSprite, 8 * pos, Transparent << 4);
				}
				// Draw piece
				if (piece.id != 0)
				{
					sprite = Byte88(pieceDefs[piece.id]->sprite);
					if (piece.team) { sprite = (sprite >> 1) & 0xf0; }
					window.layers[LayerPiece].drawSprite(sprite, 8 * pos, Transparent << 4);
				}
			}
		}

		// Clear text layer
		window.layers[LayerText].setAll(Transparent << 4);
		if (gameState == Stalemate)
		{
			window.spriteText("DRAW!", LayerText, IVec2(12, 16));
		}
		else if (gameState != Promoting)
		{
			// Flip team if game state is ended
			bool team = currTeam ^ (gameState != InProgress);
			byte fill = (team ? WhiteFill : BlackFill) << 4;
			byte outline = (team ? WhiteOutline : BlackOutline) << 4;
			window.spriteText(team ? "WHITE" : "BLACK", LayerText, IVec2(12, 8), Transparent << 4, fill, outline);

			const char* msg = (gameState == InProgress) ? " CLICK \nTO MOVE" : " WINS! ";
			window.spriteText(msg, LayerText, IVec2(4, 16));
		}
		else 
		{
			window.spriteText("PROMOTE", LayerText, IVec2(4, 0));
			int j = 0;
			for (int i = 0; i < 16; i++)
			{
				// Avoid NULL pointer
				if (pieceDefs[i] == NULL) { continue; }
				// Can't promote to self or critical
				if (board.getPiece(selectedSqr).id == i) { continue; }
				if (pieceDefs[i]->critical) { continue; }
				// Get white sprite for piece
				Byte88 sprite = (pieceDefs[i]->sprite >> 1) & 0xf0;
				// Display pieces
				window.layers[LayerText].drawSprite(sprite, IVec2(13 + 10 * (j % 4), 10 + 10 * (j / 4)), Transparent << 4);
				j++;
			}
		}

		window.invalidate();
	}

	void beginGame()
	{
		board = BoardState(startingBoard);

		gameState = InProgress;
		currTeam = 1;
		hoverSqr = IVec2(-1, -1);
		selectedSqr = IVec2(-1, -1);
		attackedCrits = Byte88();
		prvBoard = BoardState();

		calculateLegalMoves(currTeam);

		redraw();
	}

	// Begin the chess game.
	void mainloop()
	{
		beginGame();

		while (true)
		{
			window.eventTick();
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

	void finalizeMove()
	{
		Byte88 prvCrits = Byte88(attackedCrits);
		int nLegalMoves = calculateLegalMoves(currTeam);
		int nChecks = computeChecks(currTeam);

		// Check for game end
		if (nLegalMoves == 0) gameState = (nChecks != 0) ? Checkmate : Stalemate;
		else gameState = InProgress;

		selectedSqr = IVec2(-1, -1);

		redraw();
	}

	// Event handler, called during a mouse event.
	void onMouse(MOUSE_EVENT_RECORD evt)
	{
		IVec2 curPos = IVec2(evt.dwMousePosition.X, evt.dwMousePosition.Y);
		IVec2 boardPos = curPos / 8;

		if (evt.dwButtonState & RI_MOUSE_BUTTON_1_DOWN)
		{
			if ((curPos.y - 4) / 8 == 6 && curPos.x >= 64)
			{	// Clicking on restart button
				beginGame();
				return;
			}
			if (gameState == InProgress && boardPos.in88Square() && selectedSqr != boardPos)
			{
				if (board[boardPos] == 0 || board.getPiece(boardPos).team != currTeam)
				{
					if (selectedSqr.in88Square() && legalMoves[selectedSqr.y << 3 | selectedSqr.x][boardPos])
					{
						// Move piece, if promoting:
						if (makeMove(selectedSqr, boardPos)) 
						{ 
							gameState = Promoting;
							selectedSqr = boardPos;
							redraw();
						}
						else
						{
							finalizeMove();
						}
					}	
				}
				else
				{
					selectedSqr = boardPos;
					redraw();
				}
			}
			else if (gameState == Promoting)
			{
				int j = 0;
				for (int i = 0; i < 16; i++)
				{
					// Avoid NULL pointer
					if (pieceDefs[i] == NULL) { continue; }
					// Can't promote to self or critical
					if (board.getPiece(selectedSqr).id == i) { continue; }
					if (pieceDefs[i]->critical) { continue; }
					// Get piece corner position in console
					IVec2 v = IVec2(77 + 10 * (j % 4), 10 + 10 * (j / 4));
					// User clicked on this piece
					if ((curPos - v).in88Square()) 
					{
						board[selectedSqr] = (board[selectedSqr] & PIECE_TEAM) | pieceDefs[i]->id;
						finalizeMove();
						break;
					}
					j++;
				}
			}
		}
		if (evt.dwEventFlags & MOUSE_MOVED && hoverSqr != boardPos)
		{
			if (hoverSqr.in88Square() || boardPos.in88Square())
			{
				hoverSqr = hoverSqr = boardPos.in88Square() ? boardPos : IVec2(-1, -1);
				redraw();
			}
		}
	}
};

