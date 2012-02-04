#include "FrameBuffer.h"
#include "RenderView.h"

namespace RcEngine {
	namespace Render {

		FrameBuffer::FrameBuffer(unsigned int width, unsigned int height,  bool offscreen /*= true*/ )
			: mWidth(width), mHeight(height), mDepthStencilView(0), mDirty(false), mOffscreen(offscreen)
		{

		}


		FrameBuffer::~FrameBuffer(void)
		{
		}


		RenderView* FrameBuffer::GetAttachedView( unsigned int att )
		{
			switch(att)
			{
			case ATT_DepthStencil:
				return mDepthStencilView;

			default:
				unsigned int index = att - ATT_Color0;
				if(mColorViews.size() < index + 1)
					return NULL;
				else
					return mColorViews[index];
			}
		}


		void FrameBuffer::Attach( unsigned int att, RenderView* view )
		{
			switch(att)
			{
			case ATT_DepthStencil:
				if(mDepthStencilView != NULL)
				{
					Detach(ATT_DepthStencil);
				}
				mIsDepthBuffered = true;
				mDepthStencilView = view;
				PixelFormatUtils::GetNumDepthStencilBits(view->GetFormat(), mDepthBits, mStencilBits);

				break;
			default:
				unsigned int index = att - ATT_Color0;
				if(index < mColorViews.size())
				{
					if(mColorViews[index] != NULL)
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
					if(mColorViews[i] != NULL)
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

		void FrameBuffer::Detach( unsigned int att )
		{
			switch(att)
			{
			case ATT_DepthStencil:
				if(mDepthStencilView)
				{
					mDepthStencilView->OnDetach(this, att);
					mDepthStencilView = NULL;
				}

				mIsDepthBuffered = false;
				mDepthBits = 0;
				mStencilBits = 0;
				break;
			default:
				unsigned int index = att - ATT_Color0;
				if(mColorViews.size() < index + 1 )
				{
					mColorViews.resize(index + 1);
				}

				if(mColorViews[index])
				{
					mColorViews[index]->OnDetach(this, att);
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