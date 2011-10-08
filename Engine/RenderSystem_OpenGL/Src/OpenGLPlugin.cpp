#include "OpenGLPlugin.h"
#include "OpenGLRenderSystem.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{

		const String PluginName = "OpenGL RenderSystem";


		OpenGLPlugin::OpenGLPlugin(void)
			:mRenderSystem(0)
		{
		}

		OpenGLPlugin::~OpenGLPlugin(void)
		{
		}

		const String& OpenGLPlugin::GetName() const
		{
			return PluginName;
		}

		void OpenGLPlugin::Install()
		{
			mRenderSystem = new OpenGLRenderSystem();

		}

		void OpenGLPlugin::Initialise()
		{

		}

		void OpenGLPlugin::Shutdown()
		{

		}

		void OpenGLPlugin::Uninstall()
		{
			delete mRenderSystem;
			mRenderSystem = 0;
		}
	}
}