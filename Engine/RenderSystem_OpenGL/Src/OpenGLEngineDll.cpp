#ifndef _OpenGLEngineDll__H
#define _OpenGLEngineDll__H

#include "OpenGLPlugin.h"
#include "PluginManager.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{
		OpenGLPlugin *plugin = 0;

		extern "C" void _OpenGLExport dllStartPlugin(void) throw()
		{
			plugin = new OpenGLPlugin();
			PluginManager* t = PluginManager::GetSingletonPtr();
			PluginManager::GetSingleton().InstallPlugin(plugin);
		}

		extern "C" void _OpenGLExport dllStopPlugin(void)
		{	
			PluginManager::GetSingleton().UninstallPlugin(plugin);
			delete plugin;
		}
	}
}




#endif