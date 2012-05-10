#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include "Graphics/RenderDevice.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace Render
	{
		using namespace Math;

		class _OpenGLExport OpenGLRenderDevice : public RenderDevice
		{
		public:
			OpenGLRenderDevice(void);
			~OpenGLRenderDevice(void);

			void CreateRenderWindow(const RenderSettings& settings);

		    void Create();
			void Release();

			void ToggleFullscreen(bool fs);
			bool Fullscreen() const;

			void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);		
			void SetRasterizerState(const shared_ptr<RasterizerState>& state);
			void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef, uint16_t backStencilRef);

			void AdjustProjectionMatrix(Math::Matrix4f& pOut);

			//void Draw(RenderTechnique& tech, RenderOperation& operation);
			void DoRender( EffectTechnique& tech, RenderOperation& op);

		protected:
			void DoBindFrameBuffer(FrameBuffer* fb);
			
		private:
			void InitGlew();
			void BindVertexBufferOGL(const shared_ptr<GraphicsBuffer>& vertexBuffer );
			void BindIndexBufferOGL(const shared_ptr<GraphicsBuffer>& indexBuffer);
			
		private:
			
		};
	}
}


#endif // OpenGLRenderDevice_h__