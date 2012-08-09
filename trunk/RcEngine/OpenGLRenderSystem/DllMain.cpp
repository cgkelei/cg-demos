#include "OpenGLRenderModule.h"

extern "C" void _OpenGLExport dllStartPlugin(RcEngine::Core::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::Render::OpenGLRenderModule();
}

extern "C" void _OpenGLExport dllStopPlugin(RcEngine::Core::IModule** pMoudle) 
{
	delete (*pMoudle);
}
