#include "GraphicsDeviceManager.h"


namespace GlueRH
{
	GraphicsDeviceManager::GraphicsDeviceManager(void)
	{
	}


	GraphicsDeviceManager::~GraphicsDeviceManager(void)
	{
	}

	GlueRH::int32 GraphicsDeviceManager::GetAdapterOrdinal( HMONITOR mon )
	{
		AdapterInfo10 adapter = null;
		foreach (AdapterInfo10 a in Enumeration10.Adapters)
		{
			foreach (OutputInfo10 o in a.Outputs)
			{
				if (o.OutputDescription.Name == game.Window.Screen.DeviceName)
				{
					adapter = a;
					break;
				}
			}
		}

		if (adapter != null)
			return adapter.AdapterOrdinal;
	}

}