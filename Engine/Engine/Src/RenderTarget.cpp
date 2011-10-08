#include "RenderTarget.h"
#include "EngineRoot.h"
#include "RenderSystem.h"
#include "Utility.h"
#include "Camera.h"
#include "Exception.h"
#include "LogManager.h"

namespace Engine
{
	RenderTarget::RenderTarget()
		: mActive(true),
		  mPriority(OGRE_DEFAULT_RT_GROUP),
		  mHardwareGamma(false), 
		  mMutilSampleCount(0),
		  mMutilSampleQuality(0)
	{
		mViewport = new Viewport(this);
	}

	RenderTarget::~RenderTarget()
	{
		SAFE_DELETE(mViewport);
	}


	void RenderTarget::SwapBuffers( bool waitForVSync/*=true*/ )
	{

	}


	//--------------------------- 事件处理 -----------------------------------------
	void RenderTarget::FirePreUpdate( void )
	{
		RenderTargetEvent evt;
		evt.source = this;

		RenderTargetListenerList::iterator i, iend;
		i = mListeners.begin();
		iend = mListeners.end();
		for(; i != iend; ++i)
		{
			(*i)->PreRenderTargetUpdate(evt);
		}

	}

	void RenderTarget::FirePostUpdate( void )
	{
		RenderTargetEvent evt;
		evt.source = this;

		RenderTargetListenerList::iterator i, iend;
		i = mListeners.begin();
		iend = mListeners.end();
		for(; i != iend; ++i)
		{
			(*i)->PostRenderTargetUpdate(evt);
		}
	}



	void RenderTarget::RemoveListener( RenderTargetListener *listener )
	{
		RenderTargetListenerList::iterator iter = std::find(mListeners.begin(), mListeners.end(), listener);
		if(iter != mListeners.end())
		{
			mListeners.erase(iter);
		}
	}

	void RenderTarget::AddListener( RenderTargetListener *listener )
	{
		mListeners.push_back(listener);
	}

	void RenderTarget::RemoveAllListeners( void )
	{
		mListeners.clear();
	}

	//---------------------------------- Update ------------------------------------------------------
	void Engine::RenderTarget::Update( bool swapBuffers/*=true*/ )
	{
		UpdateImpl();

		if(swapBuffers)
			// 需要根据设置修改
			SwapBuffers( EngineRoot::GetSingleton().GetRenderSystem()->GetWaitForVerticalBlank());
	}

	
	void RenderTarget::BeginUpdate()
	{
		// notify listeners (pre)
		FirePreUpdate();
	}

	void RenderTarget::EndUpdate()
	{
		FirePostUpdate();
	}
	
	void RenderTarget::UpdateImpl()
	{
		BeginUpdate();
		
		mViewport->Update();

		EndUpdate();
	}

	
}

