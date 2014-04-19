#pragma once

#include "BitShiftStream.h"

#include <climits>

namespace Injectag
{

template<unsigned int SZBITS>
class ShiftRegister : public BitShiftStream
{    
public:
    ShiftRegister()
	{
		for(unsigned int i = 0; i < sizeof(m_register); i++)
			m_register[i] = 0;
	}
    
    virtual unsigned int getSizeBits() const
	{
		return SZBITS;
	}

    virtual bool peekRight()
	{
		int validRegion = CHAR_BIT - (sizeof(m_register) * CHAR_BIT - SZBITS);
		return (m_register[sizeof(m_register) - 1] & (1 << (CHAR_BIT - validRegion))) != 0;
	}

    bool peekLeft()
	{
		return (m_register[0] & (1 << (CHAR_BIT - 1))) != 0;
	}

    bool shiftRight(const bool insert)
	{
		bool nextInsertBit = insert;

		for(int i = 0; i < sizeof(m_register); i++)
		{
			int validRegion = i == sizeof(m_register) - 1 ? (CHAR_BIT - (sizeof(m_register) * CHAR_BIT - SZBITS)) : CHAR_BIT;

			bool temp = (m_register[i] & (1 << (CHAR_BIT - validRegion))) != 0;

			m_register[i] >>= 1;
			m_register[i] |= (nextInsertBit ? 1 : 0) << (CHAR_BIT - 1);
			m_register[i] &= ~(0xFF >> validRegion);

			nextInsertBit = temp;
		}

		return nextInsertBit;
	}

    bool shiftLeft(const bool insert)
	{
		bool nextInsertBit = insert;

		for(int i = sizeof(m_register) - 1; i >= 0; i--)
		{
			int validRegion = i == sizeof(m_register) - 1 ? (CHAR_BIT - (sizeof(m_register) * CHAR_BIT - SZBITS)) : CHAR_BIT;
			bool temp = (m_register[i] & (1 << (CHAR_BIT - 1))) != 0;
        
			m_register[i] <<= 1;
			m_register[i] |= (nextInsertBit ? 1 : 0) << (CHAR_BIT - validRegion);
            
			nextInsertBit = temp;
		}

		return nextInsertBit;
	}

private:

	//Statically allocated register to avoid fragmentation of heap.
    unsigned char m_register[(SZBITS + CHAR_BIT - 1) / CHAR_BIT];

};

};