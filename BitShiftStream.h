#pragma once

namespace Injectag
{

class BitShiftStream
{
public:
    
	virtual bool rollRight();
	virtual bool rollLeft();
	
    void rollRightTo(BitShiftStream& target);
	void rollLeftTo(BitShiftStream& target);

	virtual void shiftRightTo(BitShiftStream& target);
	virtual void shiftLeftTo(BitShiftStream& target);
    
	virtual unsigned int getSizeBits() const = 0;

	virtual bool peekRight() const = 0;
	virtual bool peekLeft() const = 0;

	virtual bool shiftRight(const bool insert) = 0;
	virtual bool shiftLeft(const bool insert) = 0;
};

};