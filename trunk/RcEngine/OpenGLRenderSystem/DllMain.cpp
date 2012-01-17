#include "OpenGLGraphicCommon.h"
#include "OpenGLRenderFactory.h"

extern "C" void _OpenGLExport CreateRenderFactory(RcEngine::RenderSystem::RenderFactory*& factory) 
{
	factory = new RcEngine::RenderSystem::OpenGLRenderFactory();
}