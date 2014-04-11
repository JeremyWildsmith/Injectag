#include "stdafx.h"

#include "JtagChainLinkException.h"

using namespace Injectag;

JtagChainLinkException::JtagChainLinkException(const std::string& cause) :
    runtime_error(cause)
{
}

JtagChainLinkException::~JtagChainLinkException()
{
}