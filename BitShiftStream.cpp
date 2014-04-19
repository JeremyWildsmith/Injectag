#include "stdafx.h"
#include "BitShiftStream.h"

using namespace Injectag;

bool BitShiftStream::rollRight()
{
    return shiftRight(peekRight());
}

bool BitShiftStream::rollLeft()
{
    return shiftLeft(peekLeft());
}

void BitShiftStream::rollRightTo(BitShiftStream& target)
{
	for(unsigned int i = 0; i < getSizeBits(); i++)
		target.shiftRight(rollRight());
}

void BitShiftStream::rollLeftTo(BitShiftStream& target)
{
		for(unsigned int i = 0; i < getSizeBits(); i++)
			target.shiftLeft(rollLeft());
}

void BitShiftStream::shiftRightTo(BitShiftStream& target)
{
	shiftRight(target.shiftRight(peekRight()));
}

void BitShiftStream::shiftLeftTo(BitShiftStream& target)
{
	shiftLeft(target.shiftLeft(peekLeft()));
}