#pragma once

#include "StandardModule.h"
#include "ShiftRegister.h"

namespace Injectag
{

class D2Module : public StandardModule
{
public:
	D2Module();

	virtual bool setOperation(const int operation);
	
	virtual BoundryDriver& getBounderyDriver() const;

	virtual int getManufactureId() const;
	virtual int getPartNumber() const;
	virtual int getVersion() const;

protected:
	virtual int getDrSize() const;
    virtual int getIrSize() const;

    virtual bool transmitIr(const bool dataIn);
    virtual bool transmitDr(const bool dataIn);

    virtual void linkedTo(const ChainedModule* neighbor);
    virtual void linkedFrom(const ChainedModule* neighbor);

    virtual void driveStateChanged(const DriveStage::e stage);

private:
	static const int D2_IR_SIZE = 3;
	static const int D2_BSR_SIZE = 10;

	BsrCell m_boundryCells[10];
	BoundryDriver m_boundDriver;

	ShiftRegister<D2_IR_SIZE> m_irRegister;
	ShiftRegister<D2_BSR_SIZE> m_bsrRegister;
};

};