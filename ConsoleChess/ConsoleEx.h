#pragma once
#include <iostream>
#include <Windows.h>
#include "Byte88.h"

/********************************************
* NOTE: THIS CODE WILL ONLY COMPILE ON A WINDOWS 
* SYSTEM. VISUAL STUDIO 2019 MAY BE REQUIRED.
*********************************************/

// Size of the event record buffer for a ConsoleEx object
#define SZ_RECORD_BUFFER 128

// Macro to convert RGB triplets into a 32bit integer
#define RGB(r, g, b) ((b) << 16 | (g) << 8 | (r))

// Function pointer typedefs for the event handlers.
typedef void (*KEY_EVENT_PROC)(KEY_EVENT_RECORD evt);
typedef void (*MOUSE_EVENT_PROC)(MOUSE_EVENT_RECORD evt);
typedef void (*MENU_EVENT_PROC)(MENU_EVENT_RECORD evt);
typedef void (*FOCUS_EVENT_PROC)(FOCUS_EVENT_RECORD evt);
typedef void (*BUFFER_SIZE_PROC)(WINDOW_BUFFER_SIZE_RECORD evt);

// Class implementing advanced features into the console, like events, font change, resize, etc.
class ConsoleEx
{
private:
	// Handle to console standard input
	HANDLE hConsoleIn;
	// Handle to console standard output
	HANDLE hConsoleOut;
	// Buffer of INPUT_RECORD structures containing input information
	INPUT_RECORD inputRecords[SZ_RECORD_BUFFER];
	// Default Windows colormap
	COLORREF cmapDefault[16];
public:
	KEY_EVENT_PROC onKeyEvent;		// Instance-defined key event handler
	MOUSE_EVENT_PROC onMouseEvent;  // Instance-defined mouse event handler 
	MENU_EVENT_PROC onMenuEvent;	// Instance-defined menu event handler
	FOCUS_EVENT_PROC onFocusEvent;  // Instance-defined focus event handler
	BUFFER_SIZE_PROC onBufferEvent; // Instance-defined buffer resize event handler
	COLORREF colormap[16];

	// Default CTOR
	ConsoleEx() : inputRecords{ } 
	{
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		onKeyEvent = NULL;
		onMouseEvent = NULL;
		onMenuEvent = NULL;
		onFocusEvent = NULL;
		onBufferEvent = NULL;

		SetConsoleMode(hConsoleIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
		CONSOLE_SCREEN_BUFFER_INFOEX cBuffInfo = CONSOLE_SCREEN_BUFFER_INFOEX();
		BOOL result = GetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);
		// Copy default colormap into cmapDefault and colormap
		memcpy_s(cmapDefault, sizeof(cmapDefault), cBuffInfo.ColorTable, sizeof(cmapDefault));
		memcpy_s(colormap, sizeof(colormap), cmapDefault, sizeof(colormap));
	}

	// Set console size in character rows and columns.
	BOOL setConsoleSize(int row, int col)
	{
		COORD c;
		c.X = col;
		c.Y = row;
		return SetConsoleScreenBufferSize(hConsoleOut, c);
	};

	// Call every iteration of a main loop to trigger instance-defined event handlers
	void eventTick()
	{
		DWORD nRecordsRead;
		// If input reading was sucessful
		if (ReadConsoleInput(hConsoleIn, inputRecords, SZ_RECORD_BUFFER, &nRecordsRead))
		{
			for (int i = 0; i < nRecordsRead; i++)
			{
				INPUT_RECORD record = inputRecords[i];
				switch (record.EventType)
				{
					case KEY_EVENT: 
						if (onKeyEvent != NULL) onKeyEvent(record.Event.KeyEvent); 
						break;
					case MOUSE_EVENT: 
						if (onMouseEvent != NULL) onMouseEvent(record.Event.MouseEvent); 
						break;
					case MENU_EVENT: 
						if (onMenuEvent != NULL) onMenuEvent(record.Event.MenuEvent); 
						break;
					case FOCUS_EVENT: 
						if (onFocusEvent != NULL) onFocusEvent(record.Event.FocusEvent); 
						break;
					case WINDOW_BUFFER_SIZE_EVENT: 
						if (onBufferEvent != NULL) onBufferEvent(record.Event.WindowBufferSizeEvent); 
						break;
				}
			}
		}
	}

	// Apply the current colormap to the console.
	void applyColormap()
	{
		CONSOLE_SCREEN_BUFFER_INFOEX cBuffInfo = CONSOLE_SCREEN_BUFFER_INFOEX();
		GetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);
		// Replace console colormap with ours
		memcpy_s(cBuffInfo.ColorTable, sizeof(cBuffInfo.ColorTable), colormap, sizeof(colormap));
		SetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);
	}

	// Reset the current colormap.
	void resetColormap()
	{
		memcpy_s(colormap, sizeof(colormap), cmapDefault, sizeof(colormap));
		applyColormap();
	}

	// Set the color index of the console foreground and background. 
	void setColor(int bg, int fg)
	{
		SetConsoleTextAttribute(hConsoleOut, (bg & 0xf) << 4 | (fg & 0xf));
	}

	// Set the color index of the console from an 8-bit value.
	void setColor(int col)
	{
		SetConsoleTextAttribute(hConsoleOut, col & 0xff);
	}

	// Set position at which text is written in the console.
	void setCursorPosition(int row, int col)
	{
		COORD c;
		c.X = col;
		c.Y = row;
		SetConsoleCursorPosition(hConsoleOut, c);
	}

	// Set the font used in the console and its size.
	BOOL setFont(const WCHAR* fontFace, int width, int height)
	{
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = width;  // Width of each character in the font
		cfi.dwFontSize.Y = height; // Height
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, fontFace);
		return SetCurrentConsoleFontEx(hConsoleOut, FALSE, &cfi);
	}

	// Set font to enable square space characters to draw pixel art. 
	void initSpriteMode(int pxW, int pxH)
	{
		setFont(L"Courrier New", pxW, pxH);
	}

	// Draw character sprite constitued of (BG, FG) colormap pairs in a 8x8 byte array.
	void drawSprite(Byte88 sprite, int row, int col)
	{
		for (int i = 0; i < 64; i++)
		{
			if ((i & 7) == 0) setCursorPosition(row + (i >> 3), col);
			SetConsoleTextAttribute(hConsoleOut, sprite[i]);
			printf(" ");
		}
	}

	// Draw character sprite constitued of (BG, FG) colormap pairs in a 8x8 byte array,
	// considering values of 0 to be transparent. 
	void drawSpriteAlpha(Byte88 sprite, int row, int col)
	{
		for (int i = 0; i < 64; i++)
		{
			setCursorPosition(row + (i >> 3), col + (i & 7));
			if (sprite[i] != 0)
			{
				SetConsoleTextAttribute(hConsoleOut, sprite[i]);
				printf(" ");
			}
		}
	}
};