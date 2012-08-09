#include "OISInputModule.h"

extern "C" void _InputExport dllStartPlugin(RcEngine::Core::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::Input::OISInputModule();
}