#include "stdafx.h"
#include "IdentifierModule.h"
#include "JtagChainLinkException.h"

using namespace Injectag;

IdentifierModule::ModuleId::ModuleId(const int version, const int partNumber, const int manufactureId, ModuleId* const next) :
    m_version(version),
    m_partNumber(partNumber),
    m_manufactureId(manufactureId),
    m_next(next),
    m_last(0)
{
}

IdentifierModule::ModuleId::~ModuleId()
{
    if(m_last)
        m_last->m_next = 0;

    delete m_next;
}

bool IdentifierModule::ModuleId::hasNext() const
{
    return m_next != 0;
}

IdentifierModule::ModuleId* IdentifierModule::ModuleId::getNext() const
{
    return m_next;
}

int IdentifierModule::ModuleId::getVersion() const
{
    return m_version;
}

int IdentifierModule::ModuleId::getPartNumber() const
{
    return m_partNumber;
}

int IdentifierModule::ModuleId::getManufactureId() const
{
    return m_manufactureId;
}

bool IdentifierModule::ModuleId::hasIdentity() const
{
    return m_version != 0 || m_partNumber != 0 || m_manufactureId != 0;
}

void IdentifierModule::ModuleId::linkToNew(const int version, const int partNumber, const int manufactureId)
{
    if(m_next)
        m_next->linkToNew(version, partNumber, manufactureId);
    else
        m_next = new ModuleId(version, partNumber, manufactureId);
}

IdentifierModule::IdentifierModule() :
    m_baseModule(0),
    m_hasIdentified(false),
    m_idPathRegister(ID_PATH_SIZE)
{
    for(unsigned int i = 0; i < m_idPathRegister.getSizeBits(); i++)
        m_idPathRegister.shiftRight(true);
}

IdentifierModule::~IdentifierModule()
{
    if(m_baseModule)
        delete m_baseModule;
}

bool IdentifierModule::hasIdentified() const
{
    return m_hasIdentified;
}

bool IdentifierModule::isEmpty() const
{
    return m_baseModule == 0;
}

const IdentifierModule::ModuleId* IdentifierModule::getModules() const
{
    return m_baseModule;
}

int IdentifierModule::getDrSize() const
{
    return m_hasIdentified ? 0 : m_idPathRegister.getSizeBits();
}

int IdentifierModule::getIrSize() const
{
    return 0;
}

bool IdentifierModule::transmitIr(const bool dataIn)
{
    return dataIn;
}

bool IdentifierModule::transmitDr(const bool dataOut)
{
    return m_idPathRegister.shiftRight(dataOut);
}

void IdentifierModule::linkedTo(const ChainedModule* neighbor)
{
    throw JtagChainLinkException("Identifier must maintain total control over scan path.");
}

void IdentifierModule::linkedFrom(const ChainedModule* neighbor)
{
    throw JtagChainLinkException("Identifier must maintain total control over scan path.");
}
    
void IdentifierModule::driveStateChanged(const DriveStage::e stage)
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

void IdentifierModule::appendModule(const unsigned int idCode)
{
    if(idCode == INVALID_IDCODE)
        return;

    int manufactureId = (idCode & 0xFFE) >> 1;
    int partNumber = (idCode & 0xFFFF000) >> 12;
    int version = (idCode & 0xF0000000) >> 28;

    if(m_baseModule)
        m_baseModule->linkToNew(version, partNumber, manufactureId);
    else
        m_baseModule = new ModuleId(version, partNumber, manufactureId);
}