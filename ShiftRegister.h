#pragma once

#include <climits>

namespace Injectag
{

class ShiftRegister
{    
public:
    ShiftRegister(const int sizeBits);
    virtual ~ShiftRegister();
    
    unsigned int getSizeBits() const;

    bool peekRight();
    bool peekLeft();

    bool rollRight();
    bool rollLeft();

    bool shiftRight(const bool insert);
    bool shiftLeft(const bool insert);

private:
    const int REGISTER_SIZE_BITS;
    const int REGISTER_SIZE_BYTES;

    unsigned char* m_register;

};

};