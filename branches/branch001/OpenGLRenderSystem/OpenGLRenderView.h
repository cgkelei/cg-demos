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
			virtual void ClearStencil(unsigned int stencil);
			virtual void ClearDepthStencil(float depth, unsigned int stencil);

			virtual void OnAttached(FrameBuffer* fb, unsigned int att);
			virtual void OnDetached(FrameBuffer* fb, unsigned int att);	

		protected:
			void DoClear(GLbitfield flag, const Math::ColorRGBA& clr, float depth, unsigned int stencil);

		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLDepthStencilView : public OpenGLRenderView
		{
		public:
			OpenGLDepthStencilView(unsigned int width, unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, PixelFormat format);
			OpenGLDepthStencilView(Texture* tex, unsigned int arrIndex, unsigned int level);

			~OpenGLDepthStencilView(void);

			void ClearDepth(float depth);
			void ClearStencil(unsigned int stencil);
			void ClearDepthStencil(float depth, unsigned int stencil);

			void OnAttach(FrameBuffer* fb, unsigned int index);
			void OnDetach(FrameBuffer* fb, unsigned int index);	

		private:
			GLuint mRenderBufferObject;
			GLenum mTarget;
			unsigned int mArrIndex;
			unsigned int mLevel;
			unsigned int mSampleCount, mSampleQuality;
			OpenGLTexture2D* mDepthStencilTexture;
		};

		class _OpenGLExport OpenGLScreenDepthStencilView : public OpenGLRenderView
		{
		public:
			OpenGLScreenDepthStencilView(unsigned int width, unsigned int height, PixelFormat fmt);
			~OpenGLScreenDepthStencilView();

			void OnAttach(FrameBuffer* fb, unsigned int index);
			void OnDetach(FrameBuffer* fb, unsigned int index);	
		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLRenderTarget2DView : public OpenGLRenderView
		{
		public:
			OpenGLRenderTarget2DView(Texture* tex2d, unsigned int arrIndex, unsigned int level);
			~OpenGLRenderTarget2DView();

			void ClearColor(const Math::ColorRGBA& clr);

			virtual void OnAttach(FrameBuffer* fb, unsigned int att);
			virtual void OnDetach(FrameBuffer* fb, unsigned int att);

		private:
			OpenGLTexture2D* mTexture;
			unsigned int mArrIndex;
			unsigned int mLevel;
		};

		class _OpenGLExport OpenGLScreenRenderTarget2DView :  public OpenGLRenderView
		{
		public:
			OpenGLScreenRenderTarget2DView(unsigned int width, unsigned int height, PixelFormat fmt);
			~OpenGLScreenRenderTarget2DView();
	
			void OnAttach(FrameBuffer* fb, unsigned int index);
			void OnDetach(FrameBuffer* fb, unsigned int index);
		};

	}
}
#endif // OpenGLRenderView_h__
