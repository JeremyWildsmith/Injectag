#include "stdafx.h"

#include "JtagDriver.h"

using namespace Injectag;

JtagDriver::JtagDriver(JtagHost* const host, ChainedModule* const first) :
    m_host(host),
    m_first(first)
{
    resetSequence();
}

JtagDriver::~JtagDriver()
{
}

void JtagDriver::resetSequence()
{
    m_host->set(JtagHost::InChannel::ALL, false);
    m_host->set(JtagHost::InChannel::TMS, true);

    //Move to Test-Logic-Reset
    for(int i = 0; i < 5; i++)
        m_host->clock();

    //Move To Run-Test/Idle
    m_host->set(JtagHost::InChannel::TMS, false);
    m_host->clock();

    //Move To Select-DR Scan
    m_host->set(JtagHost::InChannel::TMS, true);
    m_host->clock();
}

void JtagDriver::feedDrSequence()
{
    ChainedModule* ioTarget = m_first;
    
    //Move from Select-DR Scan to Shift-DR
    m_host->set(JtagHost::InChannel::ALL, false);

    bool movedToShiftDr = false;

    //
    for(ioTarget = m_first; ioTarget; ioTarget = ioTarget->hasNext() ? ioTarget->next() : 0)
    {
        if(ioTarget->getDrSize() <= 0)
            continue;

        if(!movedToShiftDr)
        {
            movedToShiftDr = true;
            m_host->clock();
            m_host->clock();
        }

        for(int i = 0; i < ioTarget->getDrSize(); i++)
        {
            bool currentBit = m_host->get(JtagHost::OutChannel::TDO);
            
            m_host->set(JtagHost::InChannel::TDI, ioTarget->transmitDr(currentBit));

            //Clock in last bit, with tms high if this is the last bit fed to chain.
            //This will shift in last bit and move to Exit1-Ir
            if(!ioTarget->hasNext() && i == ioTarget->getDrSize() - 1)
                m_host->set(JtagHost::InChannel::TMS, true);
            
            m_host->clock();
        }
    }
    
    if(movedToShiftDr)
    {
        //Begin moving to final destination of Select-Dr Scan
        m_host->set(JtagHost::InChannel::ALL, false);
   
        //Move to Update-DR
        m_host->set(JtagHost::InChannel::TMS, true);
        m_host->clock();

        //Move from Update-DR to Select-DR Scan
        m_host->clock();
    }
}

void JtagDriver::feedIrSequence()
{
    ChainedModule* ioTarget = m_first;
    
    //Move from Select-DR to Select-IR
    m_host->set(JtagHost::InChannel::ALL, false);
    m_host->set(JtagHost::InChannel::TMS, true);
    m_host->clock();

    //Move from Select-IR Scan to Capture-IR
    m_host->set(JtagHost::InChannel::TMS, false);
    
    bool movedToShiftIr = false;

    for(ioTarget = m_first; ioTarget; ioTarget = ioTarget->hasNext() ? ioTarget->next() : 0)
    {
        if(ioTarget->getIrSize() <= 0)
            continue;

        //Only move to shift IR if we know that we have something to shift in.
        if(!movedToShiftIr)
        {
            movedToShiftIr = true;
            m_host->clock();
            m_host->clock();
        }

        for(int i = 0; i < ioTarget->getIrSize(); i++)
        {
            bool currentBit = m_host->get(JtagHost::OutChannel::TDO);
            
            m_host->set(JtagHost::InChannel::TDI, ioTarget->transmitIr(currentBit));
            
            //Clock in last bit, with tms high if this is the last bit fed to chain.
            //This will shift in last bit and move to Exit1-Ir
            if(!ioTarget->hasNext() && i == ioTarget->getIrSize() - 1)
                m_host->set(JtagHost::InChannel::TMS, true);
            
            m_host->clock();
        }
    }
    
    if(movedToShiftIr)
    {
        //Begin moving to final destination of Select-Dr Scan
        m_host->set(JtagHost::InChannel::ALL, false);
   
        //Move to Update-DR
        m_host->set(JtagHost::InChannel::TMS, true);
        m_host->clock();

        //Move from Update-DR to Select-DR Scan
        m_host->clock();
    } else
    {
        //Begin moving to final destination of Select-Dr Scan
        m_host->set(JtagHost::InChannel::ALL, false);
   
        //Move to Update-DR
        m_host->set(JtagHost::InChannel::TMS, true);
        m_host->clock();
        
        m_host->set(JtagHost::InChannel::TMS, false);
        m_host->clock();

        m_host->set(JtagHost::InChannel::TMS, true);
        m_host->clock();
    }
}

void JtagDriver::enterDriveStage(const ChainedModule::DriveStage::e stage)
{
    for(ChainedModule* current = m_first; current; current = current->hasNext() ? current->next() : 0)
        current->enterDriveStage(stage);
}

void JtagDriver::executeTest()
{
    enterDriveStage(ChainedModule::DriveStage::FeedQuery);

    feedIrSequence();
    feedDrSequence();
    
    //Move from Select-DR to Capture-DR
    m_host->set(JtagHost::InChannel::ALL, false);
    m_host->clock();

    //Move from Capture-DR to Exit1-DR
    m_host->set(JtagHost::InChannel::TMS, true);
    m_host->clock();

    //Move from Exit1-DR to Update-DR
    m_host->clock();
    
    enterDriveStage(ChainedModule::DriveStage::Execution);
    //Move from Update-DR to Run-Test/Idle
    m_host->set(JtagHost::InChannel::TMS, false);
    m_host->clock();

    //Move from Run-Test/Idle to Select Dr-Scan
    m_host->set(JtagHost::InChannel::TMS, true);
    m_host->clock();

    enterDriveStage(ChainedModule::DriveStage::ReadBack);
    feedDrSequence();

    enterDriveStage(ChainedModule::DriveStage::End);
}

void JtagDriver::attach()
{
    resetSequence();
}

void JtagDriver::dettach()
{
    m_host->set(JtagHost::InChannel::ALL, false);
    m_host->set(JtagHost::InChannel::TMS, true);

    //Move to Test-Logic-Reset
    for(int i = 0; i < 5; i++)
        m_host->clock();
}