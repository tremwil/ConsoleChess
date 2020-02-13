#pragma once

#include <Windows.h>
#include <vector>

#include "GameWindow.h"
#include "PieceDef.h"
#include "BoardState.h"

// Sprite used for potential moves and king in check marks
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

// Enum mapping colormap indices to their use
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

// Enum mapping layer indices to their use
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

// Enum defining the different game states
enum GameState
{
	InProgress,
	Checkmate,
	Stalemate,
	Promoting
};

// Main class defining the behaviour of the chess game
class ChessGame
{
private:
	GameWindow window; // The game window

	IVec2 hoverSqr;	// Square on which the mouse is located
	IVec2 selectedSqr; // Square of selected piece
	Byte88 attackedCrits; // Used as bool array for attacked crit pieces
	Byte88 legalMoves[64]; // Array of 64 byte88s containing the legal moves for each piece for the current turn
	BoardState prvBoard; // Previous board state

	int gameState; // Current game state

	void init(std::vector<PieceDef*> pieces)
	{
		// Assign the pieces in PieceDefs at their ID
		for (int i = 0; i < pieces.size(); i++)
		{
			pieceDefs[pieces[i]->id] = pieces[i];
		}

		// Create game window and setup color-related stuff
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

		// NOTE: Change the font width and height here if the pixels are not square
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

	PieceDef* pieceDefs[16];	// Array of pointers to PieceDef
	BoardState board;			// Current board state
	BoardState startingBoard;	// Initial board state

	byte currTeam; // Current team/color

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
		// Get target piece
		Piece tgt = board.getPiece(pos);
		if (tgt.id == 0) { return false; } // Can't attack a nonexistent piece
		// Go through all squares on the board
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				IVec2 v = IVec2(i, j);
				// Check if piece at that position of the board can attack the target
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
	{	// Go through every square
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				// Check if the piece is a crit, and if yes check if it's attacked
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
	{	// setup list of attacked crit pieces
		attackedCrits = Byte88();
		int cnt = 0;
		// Go through all squares
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				// Check if piece is crit and attacked
				IVec2 v = IVec2(i, j);
				Piece p = board.getPiece(v);
				if (p.id == 0 || p.team != team) { continue; }
				if (pieceDefs[p.id]->critical && isAttacked(v))
				{	// Set to true in attackedCrits
					attackedCrits[v] = 1;
					cnt++; // Increment check counter
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
					// If move is pseudolegal, perform it and see if it leads to check
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
		// Draw selected / hover square
		window.layers[LayerSelected].transform([this](byte v, IVec2 pos)
		{
			pos /= 8; // Choose appropriate color or transparent
			return ((pos == selectedSqr)? SquareSelected : (pos == hoverSqr) ? SquareHover : Transparent) << 4;
		});
		// Clear other layers
		window.layers[LayerMoves].setAll(Transparent << 4);
		window.layers[LayerCheck].setAll(Transparent << 4);
		window.layers[LayerPiece].setAll(Transparent << 4);

		// Go through the board
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{	// Get current square position
				IVec2 pos = IVec2(i, j);
				Piece piece = board.getPiece(pos);
				Byte88 sprite;

				if (selectedSqr.x != -1 && legalMoves[selectedSqr.y << 3 | selectedSqr.x][pos])
				{	// If square is a legal move of the selected piece, draw green tgtSqrSprite
					sprite = TgtSqrSprite & 0xe0; // bitwise op. to change color
					window.layers[LayerMoves].drawSprite(sprite, 8 * pos, Transparent << 4);
				}
				if (attackedCrits[pos])
				{	// If attacked, draw red target sprite
					window.layers[LayerCheck].drawSprite(TgtSqrSprite, 8 * pos, Transparent << 4);
				}
				// Draw piece
				if (piece.id != 0)
				{
					sprite = Byte88(pieceDefs[piece.id]->sprite);
					if (piece.team) { sprite = (sprite >> 1) & 0xf0; } // Use bitwise to switch to white colors
					window.layers[LayerPiece].drawSprite(sprite, 8 * pos, Transparent << 4);
				}
			}
		}

		// Clear text layer
		window.layers[LayerText].setAll(Transparent << 4);
		if (gameState == Stalemate)
		{	// Stalement, announce the draw
			window.spriteText("DRAW!", LayerText, IVec2(12, 16));
		}
		else if (gameState != Promoting)
		{
			// Flip team if game state is ended
			bool team = currTeam ^ (gameState != InProgress);
			byte fill = (team ? WhiteFill : BlackFill) << 4;
			byte outline = (team ? WhiteOutline : BlackOutline) << 4;
			window.spriteText(team ? "WHITE" : "BLACK", LayerText, IVec2(12, 8), Transparent << 4, fill, outline);
			// Draw appropriate message depending on game state
			const char* msg = (gameState == InProgress) ? " CLICK \nTO MOVE" : " WINS! ";
			window.spriteText(msg, LayerText, IVec2(4, 16));
		}
		else 
		{	// Promotion, draw promote text and potential pieces to promote to
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
		// Invalidate window to force re-drawing
		window.invalidate();
	}

	void beginGame()
	{
		// Reset board to starting board
		board = BoardState(startingBoard);
		// Set default values for gameState, currTeam, etc.
		gameState = InProgress;
		currTeam = 1;
		hoverSqr = IVec2(-1, -1);
		selectedSqr = IVec2(-1, -1);
		attackedCrits = Byte88();
		prvBoard = BoardState();
		// Calculate legal moves of current team
		calculateLegalMoves(currTeam);
		// Redraw board
		redraw();
	}

	// Begin the chess game.
	void mainloop()
	{	// Init game
		beginGame();
		// Read key and mouse events forever
		while (true)
		{
			window.eventTick();
		}
	}

	// Event handler, called during a key press.
	void onKey(KEY_EVENT_RECORD evt)
	{
		if (evt.wVirtualKeyCode == 'Q')
		{	// Implements quitting the game using the key 'q'
			exit(0);
		}
		if (evt.wVirtualKeyCode == 'R')
		{	// Shortcut to restart the game
			beginGame();
		}
	}

	// Called to clean up the game state after a move is completely done
	void finalizeMove()
	{
		// Compute crits, and legal moves for current team
		Byte88 prvCrits = Byte88(attackedCrits);
		int nLegalMoves = calculateLegalMoves(currTeam);
		int nChecks = computeChecks(currTeam);

		// Check for game end
		if (nLegalMoves == 0) gameState = (nChecks != 0) ? Checkmate : Stalemate;
		else gameState = InProgress;
		// Deselect square
		selectedSqr = IVec2(-1, -1);
		redraw();
	}

	// Event handler, called during a mouse event.
	void onMouse(MOUSE_EVENT_RECORD evt)
	{	// Get current mouse and board position
		IVec2 curPos = IVec2(evt.dwMousePosition.X, evt.dwMousePosition.Y);
		IVec2 boardPos = curPos / 8;
		// If we are pressing left mouse button
		if (evt.dwButtonState & RI_MOUSE_BUTTON_1_DOWN)
		{
			if ((curPos.y - 4) / 8 == 6 && curPos.x >= 64)
			{	// Clicking on restart button
				beginGame();
				return;
			}
			// Clicking inside the board on a square that is not selected
			if (gameState == InProgress && boardPos.in88Square() && selectedSqr != boardPos)
			{	// Check if clicking on a non-current team square
				if (board[boardPos] == 0 || board.getPiece(boardPos).team != currTeam)
				{	// If piece is selected and we are clicking on a legal move
					if (selectedSqr.in88Square() && legalMoves[selectedSqr.y << 3 | selectedSqr.x][boardPos])
					{	// Move piece and check if promoting:
						if (makeMove(selectedSqr, boardPos)) 
						{	// Promote state, fix the selected square to the piece's new position
							gameState = Promoting;
							selectedSqr = boardPos;
							redraw();
						}
						else
						{	// No promotion, just finalize the move
							finalizeMove();
						}
					}	
				}
				else
				{	// Clicked on a current team square, change selected piece
					selectedSqr = boardPos;
					redraw();
				}
			}
			// Promotion state
			else if (gameState == Promoting)
			{	// Go through promotable pieces, calculating their icon's position
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
					// If user clicked on this piece
					if ((curPos - v).in88Square()) 
					{	// Set piece to chosen one, but keep piece team
						board[selectedSqr] = (board[selectedSqr] & PIECE_TEAM) | pieceDefs[i]->id;
						finalizeMove();
						break;
					}
					j++;
				}
			}
		}
		// if mouse was moved and not on the hoverSqr
		if (evt.dwEventFlags & MOUSE_MOVED && hoverSqr != boardPos)
		{	// If either hoverSqr or boardPos is on the board
			if (hoverSqr.in88Square() || boardPos.in88Square())
			{	// Set hoverSqr to (-1,-1) if out of bounds and boardPos otherwise
				hoverSqr = boardPos.in88Square() ? boardPos : IVec2(-1, -1);
				redraw();
			}
		}
	}
};

