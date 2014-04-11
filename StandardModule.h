#pragma once

#include "ChainedModule.h"
#include "ShiftRegister.h"

namespace Injectag
{

class StandardModule : public ChainedModule
{
protected:
    struct PinType
    {
        enum e
        {
            Input,
            Output,
            TristateOutput,
            InputOutput,
        };
    };

    class BsrMapEntry
    {
    public:
        BsrMapEntry(const int inputIndex);
        BsrMapEntry(const int outputIndex, const int controlIndex);
        BsrMapEntry(const int inputIndex, const int outputIndex, const int controlIndex);

        void overlay(ShiftRegister& bsrRegister) const;

        void setControl(const bool control);
        void setIoState(const bool ioState);
        bool getIoState() const;

        PinType::e getType() const;

    private:
        const PinType::e m_pinType;
        const int m_controlIndex;
        const int m_inputIndex;
        const int m_outputIndex;

        bool m_ioState;
        bool m_controlState;
    };

    StandardModule(const int irSize, BsrMapEntry* const bsrMap, const int bsrMapSize);
    virtual ~StandardModule();

    void samplePins();
    void externTestPins();
    void internTestPins();

    virtual void loadExternTest(ShiftRegister& irRegister) = 0;
    virtual void loadInternTest(ShiftRegister& irRegister) = 0;
    virtual void loadSample(ShiftRegister& irRegister) = 0;
};

};