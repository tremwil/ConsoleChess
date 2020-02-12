#pragma once
#define _WIN32_WINNT 0x0500

#include <iostream>
#include <Windows.h>
#include "Byte88.h"
#include "Layer.h"
#include "PixelFont.h"
#include <functional>
#include <vector>

/********************************************
* NOTE: THIS CODE WILL ONLY COMPILE ON A WINDOWS 
* SYSTEM. VISUAL STUDIO 2019 MAY BE REQUIRED.
*********************************************/

typedef std::function<void(KEY_EVENT_RECORD)> KEY_EVENT_PROC;
typedef std::function<void(MOUSE_EVENT_RECORD)> MOUSE_EVENT_PROC;
typedef std::function<void(MENU_EVENT_RECORD)> MENU_EVENT_PROC;
typedef std::function<void(FOCUS_EVENT_RECORD)> FOCUS_EVENT_PROC;
typedef std::function<void(WINDOW_BUFFER_SIZE_RECORD)> BUFFER_EVENT_PROC;

// Size of the event record buffer for a ConsoleEx object
#define SZ_RECORD_BUFFER 128

// Class implementing advanced features into the console, like events, font change, resize, etc.
class GameWindow
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

	bool ready;
	int width;
	int height;

	Layer renderBuffer;
	Layer backBuffer;

	// Set console size in character rows and columns.
	void setConsoleBufferSize(int row, int col)
	{
		COORD c;
		c.X = col + 1;
		c.Y = row + 1;
		SetConsoleScreenBufferSize(hConsoleOut, c);

		SMALL_RECT rect = SMALL_RECT();
		rect.Top = 0;
		rect.Left = 0;
		rect.Right = col;
		rect.Bottom = row;
		int ret = SetConsoleWindowInfo(hConsoleOut, true, &rect);
	};

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

public:
	KEY_EVENT_PROC onKeyEvent; // Instance-defined key event handler
	MOUSE_EVENT_PROC onMouseEvent; // Instance-defined mouse event handler 
	MENU_EVENT_PROC onMenuEvent; // Instance-defined menu event handler
	FOCUS_EVENT_PROC onFocusEvent;	// Instance-defined focus event handler
	BUFFER_EVENT_PROC onBufferEvent; // Instance-defined buffer resize event handler
	COLORREF colormap[16];

	byte textFill;
	byte textOutline;
	byte textBackground;

	byte alphaColor;

	std::vector<Layer> layers;

	// Default CTOR
	GameWindow() : inputRecords{}
	{
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		onKeyEvent = NULL;
		onMouseEvent = NULL;
		onMenuEvent = NULL;
		onFocusEvent = NULL;
		onBufferEvent = NULL;

		CONSOLE_SCREEN_BUFFER_INFOEX cBuffInfo = CONSOLE_SCREEN_BUFFER_INFOEX();
		cBuffInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(hConsoleOut, &cBuffInfo);
		// Copy default colormap into cmapDefault and colormap
		memcpy_s(cmapDefault, sizeof(cmapDefault), cBuffInfo.ColorTable, sizeof(cmapDefault));
		memcpy_s(colormap, sizeof(colormap), cmapDefault, sizeof(colormap));

		ready = false;
		textFill = 0xF0;
		textOutline = 0x70;
		textBackground = 0x00;
		alphaColor = 0x00;
	}

	void setup(int width, int height, int fontWidth, int fontHeight)
	{		
		SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ~ENABLE_QUICK_EDIT_MODE);
		setFont(L"Courrier New", fontWidth, fontHeight);

		// Block console window resizing
		HWND hWindow = GetConsoleWindow();
		SetWindowLong(hWindow, GWL_STYLE, GetWindowLong(hWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
		
		setConsoleBufferSize(height, width);

		this->width = width;
		this->height = height;
		renderBuffer = Layer(width, height, alphaColor, IVec2(0, 0));
		backBuffer = Layer(width, height, alphaColor, IVec2(0, 0));
		layers = std::vector<Layer>{ };

		ready = true;
	}

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
		cBuffInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &cBuffInfo);
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

	// Draw text as 8x8 sprites using the pixel font, relative to layer.
	void spriteText(const char* text, int layer, IVec2 pos, byte bg, byte fill, byte outline)
	{
		int i = -1;
		IVec2 cPos = pos;

		while (text[++i])
		{
			if (text[i] == '\n')
			{
				cPos.y += 8;
				cPos.x = pos.x;
				continue;
			}
			Byte88 sprite = getCharSprite(text[i], bg, fill, outline);
			layers[layer].drawSprite(sprite, cPos, alphaColor);

			cPos.x += 8;
		}
	}

	// Draw text as 8x8 sprites using the pixel font.
	void spriteText(const char* text, int layer, IVec2 pos)
	{
		spriteText(text, layer, pos, textBackground, textFill, textOutline);
	}

	// Invalidate the console window, causing it to be redrawn.
	void invalidate()
	{
		renderBuffer.setAll(alphaColor);
		for (int i = 0; i < layers.size(); i++)
		{
			renderBuffer.overlay(layers[i], alphaColor);
		}

		for (int i = 0; i < width * height; i++)
		{
			if (renderBuffer[i] != backBuffer[i])
			{
				setCursorPosition(i / width, i % width);
				setColor(renderBuffer[i]);
				printf(" ");

				backBuffer[i] = renderBuffer[i];
			}
		}
	}
};