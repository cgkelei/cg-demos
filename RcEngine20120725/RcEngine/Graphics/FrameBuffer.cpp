#include "FrameBuffer.h"
#include "RenderView.h"

namespace RcEngine {
	namespace Render {

		FrameBuffer::FrameBuffer(uint32_t width, uint32_t height,  bool offscreen /*= true*/ )
			: mWidth(width), mHeight(height), mDepthStencilView(0), mDirty(false), mOffscreen(offscreen),
			mViewport(0, 0, width, height)
		{
			
		}

		FrameBuffer::~FrameBuffer(void)
		{
			for (size_t i = 0; i < mColorViews.size(); ++i)
			{
				if (mColorViews[i])
				{
					Safe_Delete(mColorViews[i]);
				}
			}

			Safe_Delete(mDepthStencilView);
		}

		RenderView* FrameBuffer::GetAttachedView( uint32_t att )
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

		void FrameBuffer::Attach( uint32_t att, RenderView* view )
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
				uint32_t index = att - ATT_Color0;
				if(index < mColorViews.size())
				{
					if(mColorViews[index])
					{
						Detach(att);
					}
				}

				if(mColorViews.size() < index + 1 )
				{
					mColorViews.resize(index + 1);
				}

				mColorViews[index] = view;

				// Frame buffer中所有的render target的大小都一样，以最小的一个作为参考
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

			mActice = true;
			view->OnAttach(this, att);
			mDirty = true;
		}

		void FrameBuffer::Detach( uint32_t att )
		{
			switch(att)
			{
			case ATT_DepthStencil:
				if(mDepthStencilView)
				{
					mDepthStencilView->OnDetach(this, att);
					Safe_Delete(mDepthStencilView);
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
					mColorViews[index]->OnDetach(this, att);
					delete mColorViews[index];
					mColorViews[index] = NULL;
				}
			}

			mActice = true;
			mDirty = true;
		}

		void FrameBuffer::OnBind()
		{
			for (size_t i = 0; i < mColorViews.size(); ++i)
			{
				if (mColorViews[i])
				{
					mColorViews[i]->OnBind(this, ATT_Color0 + i);
				}
			}
			if (mDepthStencilView)
			{
				mDepthStencilView->OnBind(this, ATT_DepthStencil);
			}

			// Do Render API specify set
			DoBind();

			mDirty = false;
		}

		void FrameBuffer::OnUnbind()
		{
			// Do Render API specify set
			DoUnbind();

			for (size_t i = 0; i < mColorViews.size(); ++i)
			{
				if (mColorViews[i])
				{
					mColorViews[i]->OnUnbind(this, ATT_Color0 + i);
				}
			}
			if (mDepthStencilView)
			{
				mDepthStencilView->OnUnbind(this, ATT_DepthStencil);
			}
		}

	} // RenderSystem
} // RcEngine