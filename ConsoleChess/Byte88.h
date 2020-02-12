#pragma once
#include "IVec2.h"
#include <Windows.h>
#include <algorithm>

#define POS_TO_INDEX(pos) ((pos).y << 3 | (pos).x)

// Small struct equivalent to an 8x8 byte array
struct Byte88
{
	byte data[64];

	// Create empty byte8x8.
	Byte88() : data{} {};
	
	// Create copy of byte8x8.
	Byte88(const Byte88& b)
	{
		memcpy_s(data, 64, b.data, 64);
	}

	Byte88(byte b) 
	{
		std::fill_n(data, 64, b);
	};

	// Create a Byte88 from a bit board with custom LOW and HIGH bytes. 
	Byte88(UINT64 bboard, byte low, byte high) : data{ } 
	{
		for (int i = 0; i < 64; i++)
		{
			data[i] = (bboard & 1) ? high : low;
			bboard >>= 1;
		}
	}

	// Create a byte8x8 from a pointer. Unsafe.
	Byte88(byte* ptr)
	{
		memcpy_s(data, 64, ptr, 64);
	}

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