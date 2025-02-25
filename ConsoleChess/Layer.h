#pragma once

#include <stdexcept>
#include <Windows.h>
#include "IVec2.h"
#include <functional>

// Simple macro to clamp a value between a min and a max
#define CLAMP(v, mi, ma) min(max(v, mi), ma)

// 2D graphics buffer containing pixels made of bytes.
// Allows to draw sprites and overlay with other layers.
class Layer
{
protected:
	// Internal buffer containing pixel data
	byte *buffer;

	int w;	// Width of layer
	int h;	// Height of layer
	int sz; // Size of data array (w * h)

public:
	// Functions to get the width, height and size
	int width() { return w; }
	int height() { return h; }
	int size() { return sz; }

	// Position of layer relative to the window
	IVec2 pos;

	// Create empty, useless buffer2D
	Layer() : w(0), h(0), sz(0), buffer(NULL) {};

	// Create layer from a Byte88
	Layer(Byte88 b, IVec2 pos) : w(8), h(8), sz(64)
	{
		buffer = (byte*)malloc(64);
		memcpy_s(buffer, sz, b.data, sz);
	}

	// Init zero (empty) buffer2D.
	Layer(int w, int h) : w(w), h(h), sz(w * h), pos(0, 0)
	{
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, 0);
	}

	// Init Buffer2D to given value.
	Layer(int w, int h, byte val) : w(w), h(h), sz(w* h), pos(0, 0)
	{
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, val);
	}

	// Init Buffer2D to given value and position.
	Layer(int w, int h, byte val, IVec2 pos) : w(w), h(h), sz(w * h), pos(pos)
	{
		buffer = (byte*)malloc(sz);
		std::fill_n(buffer, sz, val);
	}
	
	// Init Buffer2D from data of other Buffer2D
	Layer(const Layer &other) : w(other.w), h(other.h), sz(other.sz), pos(other.pos)
	{
		buffer = (byte*)malloc(sz);
		memcpy_s(buffer, sz, other.buffer, sz);
	}

	// From pointer (unsafe)
	Layer(int w, int h, IVec2 pos, byte* data) : w(w), h(h), sz(w* h), pos(pos)
	{
		buffer = data;
	}

	// Assignment (copy) operator
	Layer& operator=(const Layer& buff)
	{	// Copy pos & size info
		pos = buff.pos;
		w = buff.w;
		h = buff.h;
		sz = buff.sz;
		// Alloc new memory and copy other buffer
		buffer = (byte*)malloc(sz);
		memcpy_s(buffer, sz, buff.buffer, sz);
		return *this; // Return object
	}

	// Destructor (free allocated memory)
	void operator~()
	{
		free(buffer);
	}

	// Resize the buffer in place to a new width and height
	void resize(int width, int height)
	{
		// Store pointer to old data
		byte* oldData = buffer;
		// Alloc new memory
		buffer = (byte*)malloc(width * height);
		// Copy every row until the max width
		for (int y = 0; y < min(height, h); h++)
		{
			memcpy_s(buffer + y * width, min(width, w), oldData + y * w, min(width, w));
		}
		free(oldData); // Free old data
		// Set new size info
		w = width;
		h = height;
		sz = width * height;
	}

	// Fill the buffer with a single value
	void setAll(byte val)
	{
		std::fill_n(buffer, sz, val);
	}

	// Replace value in buffer
	void replace(byte oldVal, byte newVal)
	{
		for (int i = 0; i < sz; i++)
		{
			if (buffer[i] == oldVal) buffer[i] = newVal;
		}
	}

	// Replace data inside buffer with that of another (no care for positi
	void replaceData(const Layer& other)
	{
		if (other.w != w || other.h != h)
			throw std::runtime_error("Cannot overlay with different size buffer");

		memcpy_s(buffer, w, other.buffer, w);
	}

	// Overlay with Byte88 at local coords
	void drawSprite(const Byte88& sprite, IVec2 layerPos, byte alpha)
	{
		// Get intersection rectangle between sprite and layer
		IVec2 start = IVec2(CLAMP(layerPos.x, 0, w), CLAMP(layerPos.y, 0, h));
		IVec2 end = IVec2(CLAMP(layerPos.x + 8, 0, w), CLAMP(layerPos.y + 8, 0, h));
		// Set every pixel in said rectangle accordingly
		for (int x = start.x; x < end.x; x++)
		{
			for (int y = start.y; y < end.y; y++)
			{
				IVec2 v = IVec2(x, y);
				if (sprite[v - layerPos] != alpha) buffer[y * w + x] = sprite[v - layerPos];
			}
		}
	}

	// Overlay with other layer at absolute coords
	void overlay(const Layer &other, byte alpha)
	{
		// Get intersection rectangle between the layers
		IVec2 start = IVec2(CLAMP(other.pos.x, pos.x, pos.x + w), CLAMP(other.pos.y, pos.y, pos.y + h));
		IVec2 end = IVec2(CLAMP(other.pos.x + other.w, pos.x, pos.x + w), CLAMP(other.pos.y + other.h, pos.y, pos.y + h));
		// Set every pixel in said rectangle accordingly
		for (int x = start.x; x < end.x; x++)
		{
			for (int y = start.y; y < end.y; y++)
			{
				IVec2 v = IVec2(x, y);
				if (other[v - other.pos] != alpha) this->operator[](v - pos) = other[v - other.pos];
			}
		}
	}

	// Apply a function on all elements of the 2D buffer.
	void transform(std::function<byte(byte)> func)
	{
		for (int i = 0; i < sz; i++)
		{
			buffer[i] = func(buffer[i]);
		}
	}

	// Apply a function on all elements of the 2D buffer, passing the index as well
	void transform(std::function<byte(byte, int)> func)
	{
		for (int i = 0; i < sz; i++)
		{
			buffer[i] = func(buffer[i], i);
		}
	}

	// Apply a function on all elements of the 2D buffer, passing the coords as well
	void transform(std::function<byte(byte, IVec2)> func)
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				buffer[y * w + x] = func(buffer[y * w + x], IVec2(x, y));
			}
		}
	}

	// Index operators (safe, throws runtime error to avoid access violation)

	byte& operator[](int i)
	{
		if (i < 0 || i > sz)
			throw std::runtime_error("Index out of range");

		return buffer[i];
	}

	byte& operator[](IVec2 v)
	{
		return this->operator[](v.y * w + v.x);
	}

	byte operator[](int i) const
	{
		return buffer[i];
	}

	byte operator[](IVec2 v) const
	{
		return this->operator[](v.y * w + v.x);
	}
};