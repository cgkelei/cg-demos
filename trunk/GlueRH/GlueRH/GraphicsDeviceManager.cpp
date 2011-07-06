#include "GraphicsDeviceManager.h"
#include "Application.h"
#include "Window.h"

namespace GlueRH
{
	GraphicsDeviceManager::GraphicsDeviceManager( ApplicationPtr app )
	{
		assert(app);
		mApplication = app;

		mApplication->EventFrameStart().bind(this, &GraphicsDeviceManager::Game_FrameStart);
		mApplication->EventFrameEnd().bind(this, &GraphicsDeviceManager::Game_FrameEnd);
		mApplication->GetWindow()->EventUserResized().bind(this, &GraphicsDeviceManager::Window_UserResized);
		mApplication->GetWindow()->EventMonitorChanged().bind(this, &GraphicsDeviceManager::Window_MonitorChanged);


	}


	GraphicsDeviceManager::~GraphicsDeviceManager(void)
	{
	}

	GlueRH::int32 GraphicsDeviceManager::GetAdapterOrdinal( HMONITOR mon )
	{
		
	}

	void GraphicsDeviceManager::EnsureD3D10()
	{
		if( !mFactory )
		{
			
		}
	}

	void GraphicsDeviceManager::InitializeDevice()
	{

	}

	void GraphicsDeviceManager::Game_FrameStart( bool* cancel )
	{

	}

	void GraphicsDeviceManager::Game_FrameEnd()
	{

	}

	void GraphicsDeviceManager::Window_UserResized()
	{

	}

	void GraphicsDeviceManager::Window_MonitorChanged()
	{

	}

	void GraphicsDeviceManager::ResizeDXGIBuffers( int width, int height, bool fullscreen )
	{

	}

	void GraphicsDeviceManager::SetupDirect3D10Views()
	{

	}

}