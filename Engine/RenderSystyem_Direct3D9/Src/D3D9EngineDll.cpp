#pragma once 

#include "D3D9Prerequisites.h"
#include "D3D9Prerequisites.h"
#include "D3D9Plugin.h"
#include "EngineRoot.h"


#ifndef BUILD_STATIC

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		D3D9Plugin *plugin = 0;

		extern "C" void _D3D9Export dllStartPlugin(void) throw()
		{
			plugin = new D3D9Plugin();
			EngineRoot::GetSingleton().InstallPlugin(plugin);
		}

		extern "C" void _D3D9Export dllStopPlugin(void)
		{	
			EngineRoot::GetSingleton().UninstallPlugin(plugin);
			delete plugin;
		}
	}
}

#endif



