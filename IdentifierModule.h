#pragma once

#include "ChainedModule.h"
#include "ShiftRegister.h"
#include "JtagChainLinkException.h"
#include <type_traits>

#include <climits>

namespace Injectag
{

template<unsigned int MAX_MODULES, unsigned int SCANPATH_IDCODE_SIZE>
class IdentifierModule : public ChainedModule
{
public:

	#pragma pack(push, 1)
    class ModuleId
    {
    public:
        bool hasNext() const
		{
			return m_next != 0;
		}

        ModuleId* getNext() const
		{
			return m_next && m_next->isValid() ? m_next : 0;
		}

        int getVersion() const
		{
			return m_version;
		}

        int getPartNumber() const
		{
			return m_partNumber;
		}

        int getManufactureId() const
		{
			return m_manufactureId;
		}

        bool hasIdentity() const
		{
			return m_version != 0 || m_partNumber != 0 || m_manufactureId != 0;
		}

    private:
        int m_version;
        int m_partNumber;
        int m_manufactureId;
        
		bool m_isValid;

        ModuleId* m_next;
        
		ModuleId() :
			m_isValid(false),
			m_manufactureId(0),
			m_partNumber(0),
			m_version(0)
		{
			m_next = this + 1;
		}
		
		bool isValid() const
		{
			return m_isValid;
		}

		void last()
		{
			m_next = 0;
		}

		void insert(const int version, const int partNumber, const int manufactureId)
		{
			if(!m_isValid)
			{
				m_version = version;
				m_partNumber = partNumber;
				m_manufactureId = manufactureId;
				m_isValid = true;
			}else if(m_next)
			{
				m_next->insert(version, partNumber, manufactureId);
			}
		}

        friend class IdentifierModule;
    };
	#pragma pack(pop)

    IdentifierModule() :
		m_hasIdentified(false)
	{
		m_moduleIds[(sizeof(m_moduleIds) / sizeof(ModuleId)) - 1].last();

		for(unsigned int i = 0; i < m_idPathRegister.getSizeBits(); i++)
			m_idPathRegister.shiftRight(true);
	}

    bool hasIdentified() const
	{
		return m_hasIdentified;
	}

    bool isEmpty() const
	{
		return !m_moduleIds[0].isValid();
	}

    //IdentifierModule _MAINTAINS OWNERSHIP_ of returned ModuleId pointer.
    const ModuleId* getModules() const
	{
		return isEmpty() ? 0 : &m_moduleIds[0];
	}

protected:
    virtual int getDrSize() const
	{
		return m_hasIdentified ? 0 : m_idPathRegister.getSizeBits();
	}

    virtual int getIrSize() const
	{
		return 0;
	}

    virtual bool transmitIr(const bool dataIn)
	{
		return dataIn;
	}

    virtual bool transmitDr(const bool dataIn)
	{
		return m_idPathRegister.shiftRight(dataIn);
	}
    
    virtual void linkedTo(const ChainedModule* neighbor)
	{
		throw JtagChainLinkException("Identifier must maintain total control over scan path.");
	}

    virtual void linkedFrom(const ChainedModule* neighbor)
	{
		throw JtagChainLinkException("Identifier must maintain total control over scan path.");
	}

    virtual void driveStateChanged(const DriveStage::e stage)
	{
		if(stage != DriveStage::Execution || m_hasIdentified)
			return;

		unsigned int idCode = 0;
		int shiftedIn = 0;

		for(unsigned int i = 0; idCode != INVALID_IDCODE && i < m_idPathRegister.getSizeBits(); i++)
		{
			bool data = m_idPathRegister.rollRight();

			if(shiftedIn > 0 || data)
			{
				idCode |= (data ? 1 : 0) << shiftedIn;
				shiftedIn++;

				if(shiftedIn == IDCODE_REGISTER_LENGTH)
				{
					appendModule(idCode);

					shiftedIn = 0;
					idCode = 0;
				}
			}else
				appendModule(idCode);
		}
        
		m_hasIdentified = true;
	}

private:
    static const int INVALID_IDCODE = 0xFFFFFFFF;
    static const int IDCODE_REGISTER_LENGTH = 32;

    ModuleId m_moduleIds[MAX_MODULES];
    bool m_hasIdentified;

    ShiftRegister<SCANPATH_IDCODE_SIZE> m_idPathRegister;

    void appendModule(const unsigned int idCode)
	{
		if(idCode == INVALID_IDCODE)
			return;

		int manufactureId = (idCode & 0xFFE) >> 1;
		int partNumber = (idCode & 0xFFFF000) >> 12;
		int version = (idCode & 0xF0000000) >> 28;

		m_moduleIds[0].insert(version, partNumber, manufactureId);
	}
};

};