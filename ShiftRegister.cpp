#include "stdafx.h"
#include "ShiftRegister.h"

using namespace Injectag;

ShiftRegister::ShiftRegister(const int sizeBits) :
    REGISTER_SIZE_BITS(sizeBits),
    REGISTER_SIZE_BYTES((sizeBits + CHAR_BIT - 1) / CHAR_BIT)
{

    m_register = new unsigned char[REGISTER_SIZE_BYTES];

    for(unsigned int i = 0; i < sizeof(m_register); i++)
        m_register[i] = 0;
}

ShiftRegister::~ShiftRegister()
{
    delete[] m_register;
}

unsigned int ShiftRegister::getSizeBits() const
{
    return REGISTER_SIZE_BITS;
}

bool ShiftRegister::peekRight()
{
    int validRegion = CHAR_BIT - (REGISTER_SIZE_BYTES * CHAR_BIT - REGISTER_SIZE_BITS);
    return (m_register[REGISTER_SIZE_BYTES - 1] & (1 << (CHAR_BIT - validRegion))) != 0;
}

bool ShiftRegister::peekLeft()
{
    return (m_register[0] & (1 << (CHAR_BIT - 1))) != 0;
}

bool ShiftRegister::rollRight()
{
    return shiftRight(peekRight());
}

bool ShiftRegister::rollLeft()
{
    return shiftLeft(peekLeft());
}

bool ShiftRegister::shiftRight(const bool insert)
{
    bool nextInsertBit = insert;

    for(int i = 0; i < REGISTER_SIZE_BYTES; i++)
    {
        int validRegion = i == REGISTER_SIZE_BYTES - 1 ? (CHAR_BIT - (REGISTER_SIZE_BYTES * CHAR_BIT - REGISTER_SIZE_BITS)) : CHAR_BIT;

        bool temp = (m_register[i] & (1 << (CHAR_BIT - validRegion))) != 0;

        m_register[i] >>= 1;
        m_register[i] |= (nextInsertBit ? 1 : 0) << (CHAR_BIT - 1);
        m_register[i] &= ~(0xFF >> validRegion);

        nextInsertBit = temp;
    }

    return nextInsertBit;
}

bool ShiftRegister::shiftLeft(const bool insert)
{
    bool nextInsertBit = insert;

    for(int i = REGISTER_SIZE_BYTES - 1; i >= 0; i--)
    {
        int validRegion = i == REGISTER_SIZE_BYTES - 1 ? (CHAR_BIT - (REGISTER_SIZE_BYTES * CHAR_BIT - REGISTER_SIZE_BITS)) : CHAR_BIT;
        bool temp = (m_register[i] & (1 << (CHAR_BIT - 1))) != 0;
        
        m_register[i] <<= 1;
        m_register[i] |= (nextInsertBit ? 1 : 0) << (CHAR_BIT - validRegion);
            
        nextInsertBit = temp;
    }

    return nextInsertBit;
}