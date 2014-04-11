#pragma once

#include <stdexcept>

namespace Injectag
{

class InvalidModuleIOStateException : public std::runtime_error
{
public:
    InvalidModuleIOStateException(const std::string& cause);
    virtual ~InvalidModuleIOStateException();
};

};