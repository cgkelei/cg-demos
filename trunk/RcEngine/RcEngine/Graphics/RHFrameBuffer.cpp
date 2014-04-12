#include <Graphics/RHFrameBuffer.h>

namespace RcEngine {

//////////////////////////////////////////////////////////////////////////
RHFrameBuffer::RHFrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: mOffscreen(offscreen),
	  mDirty(true),
	  mViewport(0, 0, width, height)  
{
	
}

RHFrameBuffer::~RHFrameBuffer()
{

}

void RHFrameBuffer::SetViewport( const RHViewport& vp )
{
	if (mViewport != vp)
	{
		mViewport = vp;
		mDirty = true;
	}
}

shared_ptr<RHRenderView> RHFrameBuffer::GetAttachedView( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		return mDepthStencilView;

	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1)
			return NULL;
		else
			return mColorViews[index];
	}
}

void RHFrameBuffer::Attach( Attachment att, const shared_ptr<RHRenderView>& view )
{
	switch(att)
	{
	case ATT_DepthStencil:
		{
			if(mDepthStencilView)
			{
				Detach(ATT_DepthStencil);
			}
			mDepthStencilView = view;
		}
		break;
	default:
		{
			uint32_t index = att - ATT_Color0;

			// if it already has an render target attach it, detach if first. 
			if(index < mColorViews.size() && mColorViews[index])
				Detach(att);

			if(mColorViews.size() < index + 1)
				mColorViews.resize(index + 1);

			mColorViews[index] = view;
		}
	}

	if (view)
		view->OnAttach(*this, att);

	mDirty = true;
}

void RHFrameBuffer::Detach( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		if(mDepthStencilView)
		{
			mDepthStencilView->OnDetach(*this, att);
			mDepthStencilView = nullptr;
		}
		break;
	default:
		uint32_t index = att - ATT_Color0;

		if(mColorViews.size() < index + 1)
			mColorViews.resize(index + 1);

		if(mColorViews[index])
		{
			mColorViews[index]->OnDetach(*this, att);
			mColorViews[index] = nullptr;
		}
	}

	mDirty = true;
}

void RHFrameBuffer::DetachAll()
{
	for (size_t i = 0; i < mColorViews.size(); ++i)
	{
		if (mColorViews[i])
		{
			Detach((Attachment)(ATT_DepthStencil + i));
		}
	}
	mColorViews.clear();

	if (mDepthStencilView)
		Detach(ATT_DepthStencil);
}

void RHFrameBuffer::Clear( uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	//RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// frame buffer must binded before clear
	//shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	//assert( this == currentFrameBuffer.get());

	if (flags & CF_Color)
	{
		for (size_t i = 0; i < mColorViews.size(); ++i)
		{
			if (mColorViews[i])
				mColorViews[i]->ClearColor(clr);	
		}
	}

	if ( mDepthStencilView )
	{
		if ( (flags & CF_Stencil) && (flags & CF_Depth) )
			mDepthStencilView->ClearDepthStencil(depth, stencil);
		else if (flags & CF_Depth)
			mDepthStencilView->ClearDepth(depth);
		else if (flags & CF_Stencil)
			mDepthStencilView->ClearStencil(stencil);
	}
}

}