#ifndef RenderView_h__
#define RenderView_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"
#include "PixelFormat.h"

namespace RcEngine {
	namespace RenderSystem {

		class _ApiExport RenderView
		{
		public: 
			RenderView();
			virtual ~RenderView();

			unsigned int GetWidth() const  { return mWidth; }
			unsigned int GetHeight() const { return mHeight; }
			PixelFormat GetFormat() const  { return mFormat; }

			virtual void ClearColor(const Math::ColorRGBA& clr) = 0;
			virtual void ClearDepth(float depth) = 0;
			virtual void ClearDepthStencil(float depth, unsigned int stencil) = 0;

			virtual void OnAttach(FrameBuffer* fb, unsigned int attr) = 0;
			virtual void OnDetach(FrameBuffer* fb, unsigned int attr) = 0;

			virtual void OnBind(FrameBuffer* fb, unsigned int att);
			virtual void OnUnbind(FrameBuffer* fb, unsigned int att);

		protected:
			unsigned int mWidth, mHeight;
			PixelFormat mFormat;
			FrameBuffer* mAttachedFrameBuffer;

		};


	} // RenderSystem
} // RcEngine



#endif // RenderView_h__
