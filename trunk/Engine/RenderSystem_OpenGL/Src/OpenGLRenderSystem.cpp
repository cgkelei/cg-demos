#include "OpenGLRenderSystem.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{

		OpenGLRenderSystem::OpenGLRenderSystem(void)
		{
		}

		OpenGLRenderSystem::~OpenGLRenderSystem(void)
		{
		}

		void OpenGLRenderSystem::ShowMessage() const
		{
			std::cout << "OpenGL RenderSystem \n";
		}

		void const String& OpenGLRenderSystem::GetName() const
		{
			static String strName( "OpenGL Rendering Subsystem");
			return strName;
		}
	}
}