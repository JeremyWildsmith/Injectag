#include "stdafx.h"

#include "ChainedModule.h"

using namespace Injectag;

ChainedModule::ChainedModule()
{
    m_next = 0;
    m_driveStage = DriveStage::End;
}

void ChainedModule::enterDriveStage(const DriveStage::e stage)
{
    m_driveStage = stage;
    driveStateChanged(stage);
}

void ChainedModule::link(ChainedModule* const next)
{
    m_next = next;
    
    linkedTo(next);
    m_next->linkedFrom(this);
}

void ChainedModule::unlink()
{
    m_next = 0;
}

ChainedModule* ChainedModule::next() const
{
    return m_next;
}

bool ChainedModule::hasNext() const
{
    return m_next != 0;
}

ChainedModule::DriveStage::e ChainedModule::getDriveStage() const
{
    return m_driveStage;
}