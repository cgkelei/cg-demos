#include <Graphics/RHFrameBuffer.h>
#include <Graphics/RHResource.h>
#include <Core/Exception.h>

namespace RcEngine {

//////////////////////////////////////////////////////////////////////////
RHRenderView::RHRenderView( const shared_ptr<RHTexture>& texture )
	: mTexture(texture)
{

}

//////////////////////////////////////////////////////////////////////////
RHFrameBuffer::RHFrameBuffer( bool offscreen /*= true*/ )
	: mOffscreen(offscreen),
	  mDirty(true)
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

shared_ptr<RHRenderView> RHFrameBuffer::GetRTV( Attachment att ) const
{
	switch(att)
	{
	case ATT_DepthStencil:
		return mDepthStencilView;

	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1)
		{
			ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Try to get render target view which it not exit!", "RHFrameBuffer::GetRenderTargetView");
		}
		else
			return mColorViews[index];
	}
}

shared_ptr<RHUnorderedAccessView> RHFrameBuffer::GetUAV( uint32_t index ) const
{
	if (index >= mUnorderedAccessViews.size())
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Try to get unordered access view which it not exit!", "RHFrameBuffer::GetUnorderedAccessView");
	
	return mUnorderedAccessViews[index];
}

void RHFrameBuffer::AttachRTV( Attachment att, const shared_ptr<RHRenderView>& view )
{
	switch(att)
	{
	case ATT_DepthStencil:
		{
			if(mDepthStencilView)
			{
				DetachRTV(ATT_DepthStencil);
			}
			mDepthStencilView = view;
		}
		break;
	default:
		{
			uint32_t index = att - ATT_Color0;

			// if it already has an render target attach it, detach if first. 
			if(index < mColorViews.size() && mColorViews[index])
				DetachRTV(att);

			if(mColorViews.size() < index + 1)
				mColorViews.resize(index + 1);

			mColorViews[index] = view;
		}
	}

	if (view)
		view->OnAttach(*this, att);

	mDirty = true;
}

void RHFrameBuffer::AttachUAV( uint32_t index, const shared_ptr<RHUnorderedAccessView>& uav )
{
	if(mUnorderedAccessViews.size() < index + 1)
		mUnorderedAccessViews.resize(index + 1);

	mUnorderedAccessViews[index] = uav;
	mDirty = true;
}

void RHFrameBuffer::DetachUAV( uint32_t index )
{
	if(mUnorderedAccessViews.size() < index + 1)
		mUnorderedAccessViews.resize(index + 1);

	mUnorderedAccessViews[index] = nullptr;
	mDirty = true;
}

void RHFrameBuffer::DetachRTV( Attachment att )
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
			DetachRTV((Attachment)(ATT_DepthStencil + i));
		}
	}
	mColorViews.clear();

	if (mDepthStencilView)
		DetachRTV(ATT_DepthStencil);

	mUnorderedAccessViews.clear();

	mDirty = true;
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