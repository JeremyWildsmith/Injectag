#pragma once

#include "ChainedModule.h"
#include "BitShiftStream.h"

namespace Injectag
{

class StandardModule : public ChainedModule
{
public:
	class BoundaryDriver;

    struct CellType
    {
        enum e
        {
            Input,
            Output,
			InputOutput,
            DriveControl,
			InternalOrIgnore,
        };
    };

	struct PinAssignment
	{
		enum e
		{
			High,
			Low,
			HighZ,
			Input,
		};
	};

	struct StandardOperation
	{
		enum e
		{
			Bypass,
			Sample,
			Extest,
			Intest,
			Last //For enum inheritance
		};
	};

    class BsrCell
    {
	public:
		BsrCell(const int pin, const CellType::e cellType, const int bsrControlBit = -1, const bool isValueInverted = false, const bool isControlInverted = false);
		BsrCell(const int pin, const CellType::e cellType, const bool isValueInverted, const bool isControlInverted = false);
		BsrCell();
        
		BsrCell& operator=(const BsrCell& rhs);

	protected:
		bool isControlled() const;

		CellType::e getCellType() const;

		//Returns -1 if pin is internal.
		int getPin() const;

		//Returns -1 if not controlled.
		int getControlBit() const;

		bool isValueActive() const;
		void setValueActive(const bool value);

		bool isControlActive() const;
		void setControlActive(const bool control);

		bool getBit() const;
        void setBit(const bool value);
	private:
		int m_pin;

		CellType::e m_cellType;

		bool m_isControlInverted;
		bool m_isValueInverted;

		bool m_value;

		int m_bsrControlBit;

		friend class BoundaryDriver;
    };

	class BoundaryDriver
	{
	public:
		BoundaryDriver(BsrCell* const cells, const int size);

		//Returns false if configuration failed due to missing cells.
		bool assignPin(const int pin, const PinAssignment::e value);

		//Returns false if sample failed due to missing cells.
		bool samplePin(const int pin, bool& outBuffer);

		BitShiftStream& getBoundaryScanRegister();
		
	protected:
		void findPin(const int pin, BsrCell*& control, BsrCell*& input, BsrCell*& output);

	private:
		class BoundaryScanRegister : public BitShiftStream
		{
        public:
            BoundaryScanRegister(BoundaryDriver* const boundaryDriver);
            virtual unsigned int getSizeBits() const;
            
            virtual bool peekRight() const;
            virtual bool peekLeft() const;

            virtual bool shiftRight(const bool insert);
            virtual bool shiftLeft(const bool insert);

		private:
			BoundaryDriver* const m_boundaryDriver;
		};

		BoundaryScanRegister m_boundaryScanRegister;
		BsrCell* const m_bsrCells;
		const int m_bsrSize;

		friend class BoundaryScanRegister;
	};

	//Returns false if operation was invalid\unsupported (defaulting to BYPASS operation), true otherwise.
	virtual bool setOperation(const int operation) = 0;
	
	//Can return null if boundry driving is unsupported.
	virtual BoundaryDriver& getBounderyDriver() const = 0;

	virtual int getManufactureId() const = 0;
	virtual int getPartNumber() const = 0;
	virtual int getVersion() const = 0;
};

};