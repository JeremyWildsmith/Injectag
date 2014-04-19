#pragma once

#include "JtagHost.h"
#include <string>

namespace Injectag
{

class VirtualJtagHost : public JtagHost
{
public:
    VirtualJtagHost();
    virtual ~VirtualJtagHost();

    virtual void set(const InChannel::e channel, bool isHigh);
    virtual bool get(const OutChannel::e channel) const;

    virtual void clock();

private:
    struct JtagState
    {
        const std::string Name;
        const int NextStateTms;
        const int NextStateNotTms;
        const bool ReadsTdo;

        JtagState(const std::string& _name, const int _nextStateTms, const int _nextStateNotTms, const bool readsTdo = false);
    };

    static const JtagState StateTable[16];

    bool m_tmsState;
    bool m_tdiState;
    bool m_tdoState;

    int m_presentStateIndex;
};

};