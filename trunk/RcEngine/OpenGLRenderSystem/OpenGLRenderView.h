#ifndef OpenGLRenderView_h__
#define OpenGLRenderView_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include "Graphics/RenderView.h"


namespace RcEngine
{
	namespace Render
	{

		class _OpenGLExport OpenGLRenderView : public RenderView
		{
		public:
			OpenGLRenderView(void);
			virtual ~OpenGLRenderView(void);

			virtual void ClearColor(const Math::ColorRGBA& clr);
			virtual void ClearDepth(float depth);
			virtual void ClearStencil(uint32_t stencil);
			virtual void ClearDepthStencil(float depth, uint32_t stencil);

			virtual void OnAttached(FrameBuffer* fb, uint32_t att);
			virtual void OnDetached(FrameBuffer* fb, uint32_t att);	

		protected:
			void DoClear(GLbitfield flag, const Math::ColorRGBA& clr, float depth, uint32_t stencil);

		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLDepthStencilView : public OpenGLRenderView
		{
		public:
			OpenGLDepthStencilView(uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, PixelFormat format);
			OpenGLDepthStencilView(Texture* tex, uint32_t arrIndex, uint32_t level);

			~OpenGLDepthStencilView(void);

			void ClearDepth(float depth);
			void ClearStencil(uint32_t stencil);
			void ClearDepthStencil(float depth, uint32_t stencil);

			void OnAttach(FrameBuffer* fb, uint32_t index);
			void OnDetach(FrameBuffer* fb, uint32_t index);	

		private:
			GLuint mRenderBufferObject;
			GLenum mTarget;
			uint32_t mArrIndex;
			uint32_t mLevel;
			uint32_t mSampleCount, mSampleQuality;
			OpenGLTexture2D* mDepthStencilTexture;
		};

		class _OpenGLExport OpenGLScreenDepthStencilView : public OpenGLRenderView
		{
		public:
			OpenGLScreenDepthStencilView(uint32_t width, uint32_t height, PixelFormat fmt);
			~OpenGLScreenDepthStencilView();

			void OnAttach(FrameBuffer* fb, uint32_t index);
			void OnDetach(FrameBuffer* fb, uint32_t index);	
		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLRenderTarget2DView : public OpenGLRenderView
		{
		public:
			OpenGLRenderTarget2DView(Texture* tex2d, uint32_t arrIndex, uint32_t level);
			~OpenGLRenderTarget2DView();

			void ClearColor(const Math::ColorRGBA& clr);

			virtual void OnAttach(FrameBuffer* fb, uint32_t att);
			virtual void OnDetach(FrameBuffer* fb, uint32_t att);

		private:
			OpenGLTexture2D* mTexture;
			uint32_t mArrIndex;
			uint32_t mLevel;
		};

		class _OpenGLExport OpenGLScreenRenderTarget2DView :  public OpenGLRenderView
		{
		public:
			OpenGLScreenRenderTarget2DView(uint32_t width, uint32_t height, PixelFormat fmt);
			~OpenGLScreenRenderTarget2DView();
	
			void OnAttach(FrameBuffer* fb, uint32_t index);
			void OnDetach(FrameBuffer* fb, uint32_t index);
		};

	}
}
#endif // OpenGLRenderView_h__
