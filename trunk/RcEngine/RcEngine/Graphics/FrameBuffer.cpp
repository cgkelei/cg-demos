#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderView.h>
#include <Graphics/RenderDevice.h>
#include <Core/Context.h>

namespace RcEngine {

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height,  bool offscreen /*= true*/ )
	: mWidth(width), mHeight(height), mDepthStencilView(0), mDirty(true), mOffscreen(offscreen),
	mViewport(0, 0, width, height)
{
			
}

FrameBuffer::~FrameBuffer(void)
{
	
}


shared_ptr<RenderView> FrameBuffer::GetAttachedView( Attachment att )
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

void FrameBuffer::Attach( Attachment att, const shared_ptr<RenderView>& view)
{
	switch(att)
	{
	case ATT_DepthStencil:
		{
			if(mDepthStencilView)
			{
				Detach(ATT_DepthStencil);
			}
			mIsDepthBuffered = true;
			mDepthStencilView = view;
			PixelFormatUtils::GetNumDepthStencilBits(view->GetFormat(), mDepthBits, mStencilBits);
		}
		break;
	default:
		{
			uint32_t index = att - ATT_Color0;

			// if it already has an render target attach it, detach if first. 
			if(index < mColorViews.size() && mColorViews[index])
			{
				Detach(att);
			}

			if(mColorViews.size() < index + 1 )
			{
				mColorViews.resize(index + 1);
			}

			mColorViews[index] = view;

			// Frame buffer�����е�render target�Ĵ�С��һ��������С��һ����Ϊ�ο�
			size_t minColorView = index;
			for(size_t i = 0; i < mColorViews.size(); ++i)
			{
				if(mColorViews[i])
				{
					minColorView = i;
				}
			}

			if(minColorView == index)
			{
				mWidth = view->GetWidth();
				mHeight = view->GetHeight();
				mColorFormat = view->GetFormat();
			}
		}
	}

	mActice = true;
	view->OnAttach(*this, att);
	mDirty = true;
}

void FrameBuffer::Detach( Attachment att )
{
	switch(att)
	{
	case ATT_DepthStencil:
		if(mDepthStencilView)
		{
			mDepthStencilView->OnDetach(*this, att);
			mDepthStencilView = nullptr;
		}

		mIsDepthBuffered = false;
		mDepthBits = 0;
		mStencilBits = 0;
		break;
	default:
		uint32_t index = att - ATT_Color0;
		if(mColorViews.size() < index + 1 )
		{
			mColorViews.resize(index + 1);
		}

		if(mColorViews[index])
		{
			mColorViews[index]->OnDetach(*this, att);
			mColorViews[index] = nullptr;
		}
	}

	mActice = true;
	mDirty = true;
}

void FrameBuffer::OnBind()
{
	// Do Render API specify set
	DoBind();

	mDirty = false;
}

void FrameBuffer::OnUnbind()
{
	// Do Render API specify set
	DoUnbind();
}

void FrameBuffer::DetachAll()
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

void FrameBuffer::Clear( uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// frame buffer must binded before clear
	shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	assert( this == currentFrameBuffer.get());

	for (size_t i = 0; i < mColorViews.size(); ++i)
	{
		if (mColorViews[i])
		{
			mColorViews[i]->ClearColor(clr);
		}		
	}

	if (mDepthStencilView)
	{
		mDepthStencilView->ClearDepthStencil(depth, stencil);
	}
}

bool FrameBuffer::CheckFramebufferStatus()
{
	return true;
}


} // Namespace RcEngine