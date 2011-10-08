#include "D3D9Plugin.h"
#include "D3D9RenderSystem.h"
#include "EngineRoot.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		const String PluginName = "Direct3D9 RenderSystem";


		D3D9Plugin::D3D9Plugin(void)
			:mRenderSystem(0)
		{
		}

		D3D9Plugin::~D3D9Plugin(void)
		{
		}

		const String& D3D9Plugin::GetName() const
		{
			return PluginName;
		}

		void D3D9Plugin::Install()
		{
			// Create the DirectX 9 rendering api

			// 根据编译类型，获取窗口的句柄
#ifdef BUILD_STATIC
			HINSTANCE hInst = GetModuleHandle( NULL );
#else
#  if ENGINE_DEBUG_MODE == 1
			HINSTANCE hInst = GetModuleHandle( "RenderSystem_Direct3D9_d.dll" );
#  else
			HINSTANCE hInst = GetModuleHandle( "RenderSystem_Direct3D9.dll" );
#  endif
#endif
			mRenderSystem = new D3D9RenderSystem( hInst );

			EngineRoot::GetSingleton().AddRenderSystem(mRenderSystem);
			
		}

		void D3D9Plugin::Initialise()
		{

		}

		void D3D9Plugin::Shutdown()
		{

		}

		void D3D9Plugin::Uninstall()
		{
			delete mRenderSystem;
			mRenderSystem = 0;
		}
	}
}