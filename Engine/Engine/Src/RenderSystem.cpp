#include "RenderSystem.h"
#include "RenderTarget.h"

namespace Engine
{

	RenderSystem::RenderSystem( void )
		: mActiveRenderTarget(NULL),
		  mActiveViewport(NULL),
		  mBatchCount(0),
		  mFaceCount(0),
		  mVertexCount(0)
	{

	}

	RenderSystem::~RenderSystem( void )
	{

	}

	RenderWindow* RenderSystem::Initialize( bool autoCreateWindow, const String& windowTitle /*= "Render Window"*/ )
	{
		return NULL;
	}

	void RenderSystem::AttachRenderTarget( RenderTarget &target )
	{
		mRenderTargets.insert( std::make_pair( target.GetName(), &target ) );
		mPrioritisedRenderTargets.insert( std::make_pair(target.GetPriority(), &target ));
	}

	void RenderSystem::Shutdown()
	{
		// Remove all the render targets.
		// (destroy primary target last since others may depend on it)
		RenderTarget* primary = 0;
		for (RenderTargetMap::iterator it = mRenderTargets.begin(); it != mRenderTargets.end(); ++it)
		{
			/*if (!primary && it->second->IsPrimary())
				primary = it->second;
			else*/
				delete it->second;
		}
		delete primary;
		mRenderTargets.clear();

		mPrioritisedRenderTargets.clear();
	}

	Viewport* RenderSystem::GetViewport()
	{
		return mActiveViewport;
	}

	RenderTarget* RenderSystem::GetRenderTarget( const String &name )
	{
		RenderTargetMapIter iter = mRenderTargets.find( name );
		RenderTarget *ret = NULL;

		if( iter != mRenderTargets.end() )
		{
			ret = iter->second;
		}

		return ret;
	}

	RenderTarget* RenderSystem::DetachRenderTarget( const String &name )
	{
		RenderTargetMapIter iter = mRenderTargets.find( name );
		RenderTarget *ret = NULL;

		if(iter != mRenderTargets.end())
		{
			 ret = iter->second;
			
			/* Remove the render target from the priority groups. */
            RenderTargetPriorityMap::iterator itarg, itargend;
            itargend = mPrioritisedRenderTargets.end();
			for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
            {
				if( itarg->second == ret ) {
					mPrioritisedRenderTargets.erase( itarg );
					break;
				}
            }

            mRenderTargets.erase( iter );
		}

		/// If detached render target is the active render target, reset active render target
		if(ret == mActiveRenderTarget)
			mActiveRenderTarget = 0;

		return ret;
	}

	void RenderSystem::DestroyRenderWindow( const String& name )
	{
		DestroyRenderTarget(name);
	}

	void RenderSystem::DestroyRenderTarget( const String& name )
	{
		RenderTarget* ret = DetachRenderTarget(name);
		delete ret;
	}

	void RenderSystem::SetWorldMatrices( const Matrix4f* m, unsigned short count )
	{

	}

	void RenderSystem::SetWaitForVerticalBlank( bool enabled )
	{
		mVSync = enabled;
	}

	bool RenderSystem::GetWaitForVerticalBlank( void ) const
	{
		return mVSync;
	}

	void RenderSystem::UpdateAllRenderTargets( bool swapBuffers /*= true*/ )
	{
		// Update all in order of priority
		// This ensures render-to-texture targets get updated before render windows
		RenderTargetPriorityMap::iterator itarg, itargend;
		itargend = mPrioritisedRenderTargets.end();
		for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
		{
			if( itarg->second->IsActive() )
				itarg->second->Update(swapBuffers);
		}
	}
}