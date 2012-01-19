#include "OpenGLRenderModule.h"

extern "C" void _OpenGLExport dllStartPlugin(RcEngine::Core::IModule** pMoudle) 
{
	*pMoudle = new RcEngine::RenderSystem::OpenGLRenderModule();
}