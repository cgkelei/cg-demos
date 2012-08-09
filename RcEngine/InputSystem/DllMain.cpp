#include "OISInputModule.h"

extern "C" void _InputExport dllStartPlugin(RcEngine::Core::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::Input::OISInputModule();
}

extern "C" void _InputExport dllStopPlugin(RcEngine::Core::IModule** pMoudle) 
{
	delete (*pMoudle);
}
