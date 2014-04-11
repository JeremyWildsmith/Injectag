#pragma once

#include "JtagHost.h"
#include "ChainedModule.h"

namespace Injectag
{

class JtagDriver
{
public:
    JtagDriver(JtagHost* const host, ChainedModule* const first);
    virtual ~JtagDriver();
    
    void executeTest();
    void attach();
    void dettach();

private:
    JtagHost* const m_host;
    ChainedModule* const m_first;
    bool m_isAttached;

    void resetSequence();

    void feedDrSequence();
    void feedIrSequence();

    void enterDriveStage(const ChainedModule::DriveStage::e stage);
};

};