// INjECtag.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ShiftRegister.h"
#include "VirtualJtagHost.h"
#include "JtagDriver.h"
#include "IdentifierModule.h"

#include <iostream>

using namespace Injectag;

void fuck(int arra[])
{

}

int _tmain(int argc, _TCHAR* argv[])
{
	typedef IdentifierModule<10, 35> IdMod;

	VirtualJtagHost host;
    IdMod module;
    JtagDriver driver(&host, &module);
    driver.executeTest();

    for(const IdMod::ModuleId* pModule = module.getModules(); pModule; pModule = pModule->hasNext() ? pModule->getNext() : 0)
    {
        printf("%s ManID: 0x%X Part#: 0x%X Ver: 0x%X\n", pModule->hasIdentity() ? "Identified" : "Unidentified", pModule->getManufactureId(), pModule->getPartNumber(), pModule->getVersion());
    }

	return 0;
}
