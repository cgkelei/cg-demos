#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderOperation.h"
#include "Graphics/BlendState.h"
#include "Graphics/DepthStencilState.h"
#include "Graphics/SamplerState.h"
#include "Graphics/RasterizerState.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderSettings.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"

namespace RcEngine {
	namespace Render {

		using namespace Math;

		class _ApiExport RenderDevice
		{
		public:
			RenderDevice(void);
			virtual ~RenderDevice(void);

			RenderFactory* GetRenderFactory() const;

			void BindFrameBuffer(FrameBuffer* fb);
			FrameBuffer* GetCurrentFrameBuffer();
			FrameBuffer* GetDefaultFrameBuffer();

			virtual void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32 sampleMask) = 0;
			/*virtual void SetSamplerState(const shared_ptr<SamplerState>& state);*/
			virtual void SetRasterizerState(const shared_ptr<RasterizerState>& state) = 0;
			virtual void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16 frontStencilRef, uint16 backStencilRef) = 0;

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

			shared_ptr<BlendState> mCurrentBlendState;
			shared_ptr<SamplerState> mCurrentSamplerState;
			shared_ptr<RasterizerState> mCurrentRasterizerState;
			shared_ptr<DepthStencilState> mCurrentDepthStencilState;

			ColorRGBA mCurrentBlendFactor;
			uint32 mCurrentSampleMask;

			uint16 mCurrentFrontStencilRef, mCurrentBackStencilRef;
			
			

		};

	}
}

#endif // RenderDevice_h__