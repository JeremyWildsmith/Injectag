#include "stdafx.h"
#include "VirtualJtagHost.h"
#include <iostream>
#include <iomanip>

#include <Windows.h>

using namespace Injectag;

const VirtualJtagHost::JtagState VirtualJtagHost::StateTable[16] = {  VirtualJtagHost::JtagState("Test_Logic_Reset", 0, 1),
                                                                        VirtualJtagHost::JtagState("Run_Test_Idle", 2, 1),
                                                                        VirtualJtagHost::JtagState("Select_DR", 9, 3), //2
                                                                        VirtualJtagHost::JtagState("Capture_DR", 5, 4),
                                                                        VirtualJtagHost::JtagState("Shift_DR", 5, 4, true),
                                                                        VirtualJtagHost::JtagState("Exit1_DR", 8, 6),
                                                                        VirtualJtagHost::JtagState("Pause_DR", 7, 6), //6
                                                                        VirtualJtagHost::JtagState("Exit2_DR", 8, 4),
                                                                        VirtualJtagHost::JtagState("Update_DR", 2, 1), //8
                                                                        VirtualJtagHost::JtagState("Select_IR", 0, 10),
                                                                        VirtualJtagHost::JtagState("Capture_IR", 12, 11), //10
                                                                        VirtualJtagHost::JtagState("Shift_IR", 12, 11, true),
                                                                        VirtualJtagHost::JtagState("Exit1_IR", 15, 13), //12
                                                                        VirtualJtagHost::JtagState("Pause_IR", 14, 13),
                                                                        VirtualJtagHost::JtagState("Exit2_IR", 15, 11), //14
                                                                        VirtualJtagHost::JtagState("Update_IR", 2, 1)
                                                                        };

VirtualJtagHost::JtagState::JtagState(const std::string& _name, const int _nextStateTms, const int _nextStateNotTms, const bool _readsTdo) :
    Name(_name),
    NextStateTms(_nextStateTms),
    NextStateNotTms(_nextStateNotTms),
    ReadsTdo(_readsTdo)
{
}

VirtualJtagHost::VirtualJtagHost() :
    m_tmsState(false),
    m_tdiState(false)
{
    m_presentStateIndex = 0;
}

VirtualJtagHost::~VirtualJtagHost()
{
}


void VirtualJtagHost::set(const InChannel::e channel, bool isHigh)
{
    switch(channel)
    {
    case InChannel::TDI:
        m_tdiState = isHigh;
        break;
    case InChannel::TMS:
        m_tmsState = isHigh;
        break;
    case InChannel::ALL:
        m_tmsState = m_tdiState = isHigh;
        break;
    default:
        break;
    }
}

bool VirtualJtagHost::get(const OutChannel::e channel) const
{
    return m_tdoState;
}

void VirtualJtagHost::clock()
{
    int lastState = m_presentStateIndex;
    m_presentStateIndex = m_tmsState ? StateTable[m_presentStateIndex].NextStateTms : StateTable[m_presentStateIndex].NextStateNotTms; 

    std::cout<<StateTable[lastState].Name<<" to "<<StateTable[m_presentStateIndex].Name<<std::endl;
    std::cout<<"\t (TDI): "<<(m_tdiState ? "1" : "0")<<" State of (TDO): ";

    if(StateTable[m_presentStateIndex].ReadsTdo)
    {
        std::string inBuffer;
 
        std::cin>>inBuffer;
        std::cin.ignore();

        m_tdoState = inBuffer.compare("1") == 0;
    }else
        std::cout<<"X"<<std::endl;

    std::cout<<std::endl;
}