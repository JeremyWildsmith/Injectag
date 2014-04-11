#pragma once

#include "ChainedModule.h"
#include "ShiftRegister.h"

#include <climits>

namespace Injectag
{

class IdentifierModule : public ChainedModule
{
public:
    class ModuleId
    {
    public:
        bool hasNext() const;
        ModuleId* getNext() const;

        int getVersion() const;
        int getPartNumber() const;
        int getManufactureId() const;

        bool hasIdentity() const;

    private:
        const int m_version;
        const int m_partNumber;
        const int m_manufactureId;
        
        ModuleId* m_next;
        ModuleId* m_last;
        
        ModuleId(const int version, const int partNumber, const int manufatureId, ModuleId* const next = 0);
        virtual ~ModuleId();

        void linkToNew(const int version = 0, const int partNumber = 0, const int manufactureId = 0);

        friend class IdentifierModule;
    };

    IdentifierModule();
    virtual ~IdentifierModule();

    bool hasIdentified() const;
    bool isEmpty() const;

    //IdentifierModule _MAINTAINS OWNERSHIP_ of returned ModuleId pointer.
    const ModuleId* getModules() const;

protected:
    virtual int getDrSize() const;
    virtual int getIrSize() const;

    virtual bool transmitIr(const bool dataIn);
    virtual bool transmitDr(const bool dataIn);
    
    virtual void linkedTo(const ChainedModule* neighbor);
    virtual void linkedFrom(const ChainedModule* neighbor);

    virtual void driveStateChanged(const DriveStage::e stage);

private:
    static const int ID_PATH_SIZE = CHAR_BIT * 30;
    static const int INVALID_IDCODE = 0xFFFFFFFF;
    static const int IDCODE_REGISTER_LENGTH = 32;

    ModuleId* m_baseModule;
    bool m_hasIdentified;

    ShiftRegister m_idPathRegister;

    void appendModule(const unsigned int idcode);
};

};