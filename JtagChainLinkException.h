#pragma once

#include <stdexcept>

namespace Injectag
{

class JtagChainLinkException : public std::runtime_error
{
public:
    JtagChainLinkException(const std::string& cause);
    virtual ~JtagChainLinkException();
};

};