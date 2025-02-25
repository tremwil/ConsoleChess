#pragma once
#include "IVec2.h"
#include <Windows.h>

// Macro to convert a board vector position to its index
#define POS_TO_INDEX(pos) ((pos).y << 3 | (pos).x)

// Small struct equivalent to an 8x8 byte array.
// Used to store sprites, boards, etc.
struct Byte88
{
	byte data[64]; // Internal data of the Byte88

	// Create empty byte88.
	Byte88() : data{} {};
	
	// Create copy of byte88.
	Byte88(const Byte88& b)
	{	// Copy data of b to this->data
		memcpy_s(data, 64, b.data, 64);
	}

	// Create Byte88 initalized with a single value
	Byte88(byte b) 
	{	// use fill_n to fill data buffer with 64 copies of b
		std::fill_n(data, 64, b);
	};

	// Create a Byte88 from a bit board with custom LOW and HIGH bytes. 
	Byte88(UINT64 bboard, byte low, byte high) : data{ } 
	{	// Go through bitboard bit by bit
		for (int i = 0; i < 64; i++)
		{	// Read LSB bit and assign to low/high
			data[i] = (bboard & 1) ? high : low;
			bboard >>= 1; // Shift bitboard right
		}
	}

	// Create a byte8x8 from a pointer. Unsafe.
	Byte88(byte* ptr)
	{	// Copy 64 bytes from ptr to this->data
		memcpy_s(data, 64, ptr, 64);
	}

	// OPERATORS
	// All operators are defined in a similar fashion. For each operation,
	// we implement the operator (as const) and its assignment variant (ie. +=).
	// Every operation is made element-wise with another Byte88 using a for loop.
	// assignment operators return the object.

	Byte88& operator+= (Byte88 & b) 
	{										
		for (int i = 0; i < 64; i++)		
		{									
			data[i] += b.data[i];		
		}									
		return *this;						
	}	
	
	Byte88 operator+ (Byte88 & b) const
	{			
		Byte88 cpy = Byte88(*this);
		cpy += b;
		return cpy;
	}

	Byte88 operator-() const
	{
		Byte88 cpy = Byte88(*this);
		for (int i = 0; i < 64; i++)
		{
			cpy.data[i] = -data[i];
		}
		return cpy;
	}

	Byte88& operator-= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] -= b.data[i];
		}
		return *this;
	}

	Byte88 operator- (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy -= b;
		return cpy;
	}

	Byte88& operator*= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] *= b.data[i];
		}
		return *this;
	}

	Byte88 operator* (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy *= b;
		return cpy;
	}

	Byte88& operator/= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] /= b.data[i];
		}
		return *this;
	}

	Byte88 operator/ (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy /= b;
		return cpy;
	}

	Byte88& operator|= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] |= b.data[i];
		}
		return *this;
	}

	Byte88 operator| (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy |= b;
		return cpy;
	}

	Byte88& operator|= (byte b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] |= b;
		}
		return *this;
	}

	Byte88 operator| (byte b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy |= b;
		return cpy;
	}

	Byte88& operator&= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] &= b.data[i];
		}
		return *this;
	}

	Byte88 operator& (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy &= b;
		return cpy;
	}

	Byte88& operator&= (byte b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] &= b;
		}
		return *this;
	}

	Byte88 operator& (byte b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy &= b;
		return cpy;
	}

	Byte88& operator^= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] ^= b.data[i];
		}
		return *this;
	}

	Byte88 operator^ (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy ^= b;
		return cpy;
	}

	Byte88& operator^= (byte b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] ^= b;
		}
		return *this;
	}

	Byte88 operator^ (byte b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy ^= b;
		return cpy;
	}

	Byte88& operator>>= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] >>= b.data[i];
		}
		return *this;
	}

	Byte88 operator>> (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy >>= b;
		return cpy;
	}

	Byte88& operator>>= (byte b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] >>= b;
		}
		return *this;
	}

	Byte88 operator>> (byte b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy >>= b;
		return cpy;
	}

	Byte88& operator<<= (Byte88& b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] <<= b.data[i];
		}
		return *this;
	}

	Byte88 operator<< (Byte88& b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy <<= b;
		return cpy;
	}

	Byte88& operator<<= (byte b)
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] <<= b;
		}
		return *this;
	}

	Byte88 operator<< (byte b) const
	{
		Byte88 cpy = Byte88(*this);
		cpy <<= b;
		return cpy;
	}

	Byte88 operator~() const
	{
		Byte88 cpy = Byte88(*this);
		for (int i = 0; i < 64; i++)
		{
			cpy.data[i] = ~cpy.data[i];
		}
		return cpy;
	}

	// INDEX OPERATORS
	// Index operators are defined for both integer and IVec2 positions.
	// One version returns a reference to the value so that one can 
	// assign an index expression, ex. byte88[5] = 3. There is also a const version
	// so that one can properly use to constant type Byte88 object.

	byte& operator[](int index)
	{
		return data[index];
	}

	byte& operator[](IVec2 pos)
	{
		return data[pos.y << 3 | pos.x];
	}

	byte operator[](int index) const
	{
		return data[index];
	}

	byte operator[](IVec2 pos) const
	{
		return data[pos.y << 3 | pos.x];
	}
};