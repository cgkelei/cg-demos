#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderOperation.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderSettings.h"

namespace RcEngine {
	namespace Render {

		class _ApiExport RenderDevice
		{
		public:
			RenderDevice(void);
			virtual ~RenderDevice(void);

			RenderFactory* GetRenderFactory() const;

			void BindFrameBuffer(FrameBuffer* fb);
			FrameBuffer* GetCurrentFrameBuffer();
			FrameBuffer* GetDefaultFrameBuffer();

			virtual void Create() = 0;
			virtual void Release() = 0;

			virtual void ToggleFullscreen(bool fs) = 0;
			virtual bool Fullscreen() const = 0;

			virtual void CreateRenderWindow(const RenderSettings& settings) = 0;

			virtual void Draw(RenderTechnique& tech, RenderOperation& operation) = 0;

			void Resize(unsigned int width, unsigned int height);

			virtual void AdjustProjectionMatrix(Math::Matrix4f& pOut) = 0;

		protected:
			virtual void DoBindFrameBuffer(FrameBuffer* fb) = 0;

			virtual void BindOutputStreams(RenderOperation& operation) = 0;

		protected:

			unsigned int mWidth, mHeight;

			PixelFormat mColorFormat;
			unsigned int mColorBits;

			unsigned int mDepthBits, mStencilBits;
			bool mIsDepthBuffered;

			FrameBuffer* mCurrentFrameBuffer;
			FrameBuffer* mDefaultFrameBuffer;

			RenderSettings mRenderSettings;

			Viewport mViewport;

			RenderFactory* mRenderFactory;
		};

	}
}

#endif // RenderDevice_h__