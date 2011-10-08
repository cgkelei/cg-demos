#include "ContextCfg.h"
#include "LogManager.h"
#include "Exception.h"
#include "PixelFormat.h"

namespace Engine
{
	SINGLETON_DECL(ContextCfg)

	ContextCfg::ContextCfg()
	{

	}

	ContextCfg::~ContextCfg()
	{

	}

	void ContextCfg::LoadConfig( String path )
	{

	}

	void ContextCfg::SaveConfig( String path )
	{

	}

	void ContextCfg::BuildDefaultConfig()
	{
		mRenderSystemName = "Direct3D9 RenderSystem";
		mRenderSettings.Fullscreen = false;
		mRenderSettings.ColorFormat = PF_A8R8G8B8;
		mRenderSettings.DepthStencilForamt = DF_Depth24Stencil8;
		mRenderSettings.Height = 480;
		mRenderSettings.Width = 640;
		mRenderSettings.Left = 100;
		mRenderSettings.Top = 100;
		mRenderSettings.SampleCount = 0;
		mRenderSettings.SampleQuality = 0;
		mRenderSettings.SyncInterval = 0;
	}
}