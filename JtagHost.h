#pragma once


namespace Injectag
{

class JtagHost
{
public:
    struct InChannel
    {
        enum e
        {
            TDI,
            TMS,
            ALL,
        };
    };

    struct OutChannel
    {
        enum e
        {
            TDO
        };
    };

    virtual void set(const InChannel::e channel, bool isHigh) = 0;
    virtual bool get(const OutChannel::e channel) const = 0;
    virtual void clock() = 0;
};

};