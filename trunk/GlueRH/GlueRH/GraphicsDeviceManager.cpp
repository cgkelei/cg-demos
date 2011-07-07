#include "GraphicsDeviceManager.h"
#include "Application.h"
#include "Window.h"
#include "Direct3DUtil.h"


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
		return -1;
	}

	void GraphicsDeviceManager::EnsureD3D10()
	{
		if(mFactory == NULL)
		{
			
		}
	}

	void GraphicsDeviceManager::InitializeDevice()
	{	
		HR( CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&mFactory) ) );

		// Allows DXGI to monitor an application's message queue for the alt-enter key sequence 
		// Which causes the application to switch from windowed to fullscreen or vice versa.
		mFactory->MakeWindowAssociation(0, 0);
		
		IDXGIAdapter* adapter = NULL;
		if (mCurrentSettings.Direct3D10.DriverType == D3D10_DRIVER_TYPE_HARDWARE)
			assert( SUCCEEDED( mFactory->EnumAdapters(mCurrentSettings.AdapterOrdinal, &adapter) ) );

		
		HR( D3D10CreateDeviceAndSwapChain(adapter, mCurrentSettings.Direct3D10.DriverType, NULL, mCurrentSettings.Direct3D10.CreationFlags,
											D3D10_SDK_VERSION, &mCurrentSettings.Direct3D10.SwapChainDescription, &mSwapChain, &mD3D10Device ) );
		
		if (adapter != NULL )
		{
			SafeRelease(&adapter);
		}

		SetupDirect3D10Views();
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