#include "stdafx.h"

#include "StandardModule.h"

using namespace Injectag;

StandardModule::BsrCell::BsrCell(const int pin, const CellType::e cellType, const int bsrControlBit, const bool isValueInverted, const bool isControlInverted) :
	m_pin(pin),
	m_cellType(cellType),
	m_value(false),
	m_bsrControlBit(bsrControlBit),
	m_isValueInverted(isValueInverted),
	m_isControlInverted(isControlInverted)
{
}

StandardModule::BsrCell::BsrCell(const int pin, const CellType::e cellType, const bool isValueInverted, const bool isControlInverted) :
	m_pin(pin),
	m_cellType(cellType),
	m_value(false),
	m_bsrControlBit(-1),
	m_isValueInverted(isValueInverted),
	m_isControlInverted(isControlInverted)
{
}

StandardModule::BsrCell::BsrCell() :
	m_pin(-1),
	m_cellType(CellType::InternalOrIgnore),
	m_bsrControlBit(-1),
	m_isValueInverted(false),
	m_isControlInverted(false)
{
}

StandardModule::BsrCell& StandardModule::BsrCell::operator=(const StandardModule::BsrCell& rhs)
{
	m_pin = rhs.m_pin;
	m_value = rhs.m_value;
	m_cellType = rhs.m_cellType;
	m_bsrControlBit = rhs.m_bsrControlBit;
	m_isControlInverted = rhs.m_isControlInverted;
	m_isValueInverted = rhs.m_isValueInverted;
	return *this;
}

int StandardModule::BsrCell::getPin() const
{
	return m_cellType == CellType::InternalOrIgnore ? -1 : m_pin;
}

int StandardModule::BsrCell::getControlBit() const
{
	return isControlled() ? -1 : m_bsrControlBit;
}

bool StandardModule::BsrCell::isControlled() const
{
	return m_cellType == CellType::InternalOrIgnore || m_cellType == CellType::DriveControl ? false : m_bsrControlBit < 0;
}

StandardModule::CellType::e StandardModule::BsrCell::getCellType() const
{
	return m_cellType;
}

bool StandardModule::BsrCell::isValueActive() const
{
	return m_isValueInverted ? !m_value : m_value;
}

void StandardModule::BsrCell::setValueActive(const bool value)
{
	m_value = m_isValueInverted ? !value : value;
}

bool StandardModule::BsrCell::isControlActive() const
{
	return m_isControlInverted ? !m_value : m_value;
}

void StandardModule::BsrCell::setControlActive(const bool control)
{
	m_value = m_isControlInverted ? !control : control;
}

bool StandardModule::BsrCell::getBit() const
{
	return m_value;
}

void StandardModule::BsrCell::setBit(const bool value)
{
    m_value = value;
}

StandardModule::BoundaryDriver::BoundaryScanRegister::BoundaryScanRegister(BoundaryDriver* const boundaryDriver) :
    m_boundaryDriver(boundaryDriver),
    m_currentCell(0)
{

}

unsigned int StandardModule::BoundaryDriver::BoundaryScanRegister::getSizeBits() const
{
    return m_boundaryDriver->m_bsrSize;
}

bool StandardModule::BoundaryDriver::BoundaryScanRegister::peekRight() const
{
    int size = m_boundaryDriver->m_bsrSize;

    if(size <= 0)
        return false;
    else
        return m_boundaryDriver->m_bsrCells[size - 1].getBit();
}

bool StandardModule::BoundaryDriver::BoundaryScanRegister::shiftRight(const bool insert)
{
    bool out = insert;

    for(int i = 0; i < m_boundaryDriver->m_bsrSize; i++)
    {
        BsrCell current = m_boundaryDriver->m_bsrCells[i];

        bool temp = current.getBit();
        current.setBit(out);
        out = temp;
    }

    return out;
}

bool StandardModule::BoundaryDriver::BoundaryScanRegister::shiftLeft(const bool insert)
{
    bool out = insert;
    
    for(int i = m_boundaryDriver->m_bsrSize - 1; i >= 0; i--)
    {
        BsrCell current = m_boundaryDriver->m_bsrCells[i];

        bool temp = current.getBit();
        current.setBit(out);
        out = temp;
    }

    return out;
}

bool StandardModule::BoundaryDriver::BoundaryScanRegister::peekLeft() const
{
    if(m_boundaryDriver->m_bsrSize <= 0)
        return false;
    return
        m_boundaryDriver->m_bsrCells[0].getBit();
}

StandardModule::BoundaryDriver::BoundaryDriver(BsrCell* const cells, const int size) :
	m_bsrCells(cells),
	m_bsrSize(size),
    m_boundaryScanRegister(this)
{
};

bool StandardModule::BoundaryDriver::assignPin(const int pin, const PinAssignment::e assignment)
{
	BsrCell* driveControl = 0;
	BsrCell* input = 0;
	BsrCell* output = 0;

	findPin(pin, driveControl, input, output);

	switch(assignment)
	{
	case PinAssignment::Low:
	case PinAssignment::High:
		if(!output)
			return false;
		
		if(driveControl)
			driveControl->setControlActive(true);
		
		output->setValueActive(assignment == PinAssignment::High);
		break;
	case PinAssignment::Input:
	case PinAssignment::HighZ:
		if(!driveControl)
			return false;

		driveControl->setControlActive(false);
		break;
	default:
		return false;
	}

	return true;
}

bool StandardModule::BoundaryDriver::samplePin(const int pin, bool& outBuffer)
{
	BsrCell* driveControl = 0;
	BsrCell* input = 0;
	BsrCell* output = 0;

	findPin(pin, driveControl, input, output);

	if(driveControl)
	{
		//If bidir cell with control
		if(input == output)
			outBuffer = input->isValueActive();
		else if(driveControl->isControlActive())
		{
			if(!output)
				return false;

			outBuffer = output->isValueActive();
		}else
		{
			if(!input)
				return false;

			outBuffer = input->isValueActive();
		}
	}else if(input && !output)
		outBuffer = input->isValueActive();
	else if(output && !input)
		outBuffer = output->isValueActive();
	else
		return false;

	return true;
}

BitShiftStream& StandardModule::BoundaryDriver::getBoundaryScanRegister()
{
    return m_boundaryScanRegister;
}

void StandardModule::BoundaryDriver::findPin(const int pin, BsrCell*& driveControl, BsrCell*& input, BsrCell*& output)
{
	int controlCell = -1;

	for(int i = 0; i < m_bsrSize; i++)
	{
		if(m_bsrCells[i].getPin() == pin)
		{
			switch(m_bsrCells[i].getCellType())
			{
			case CellType::Input:
				input = &m_bsrCells[i];
				
				if(input->isControlled())
					controlCell = input->getControlBit();

				break;
			case CellType::Output:
				output = &m_bsrCells[i];

				if(input->isControlled())
					controlCell = input->getControlBit();

				break;
			case CellType::InputOutput:
				output = &m_bsrCells[i];
				input = &m_bsrCells[i];
				
				if(input->isControlled())
					controlCell = input->getControlBit();

				break;
			}
		}
	}

	if(controlCell >= 0 && controlCell < sizeof(m_bsrCells) / sizeof(BsrCell))
		driveControl = &m_bsrCells[controlCell];
}