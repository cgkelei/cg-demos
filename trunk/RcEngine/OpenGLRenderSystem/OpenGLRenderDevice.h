#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include "Graphics/RenderDevice.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		class _OpenGLExport OpenGLRenderDevice : public RenderDevice
		{
		public:
			OpenGLRenderDevice(void);
			~OpenGLRenderDevice(void);

			void CreateRenderWindow(const RenderSettings& settings);

		    void Create();
			void Release();

			void BeginFrame();
			void EndFrame();

			void ToggleFullscreen(bool fs);
			bool IsFullscreen() const;

			void AdjustProjectionMatrix(Math::Matrix4f& pOut);
			void Draw(RenderEffect* effect, const RenderOperation& operation);

			void BindVertexBuffer(const GraphicBuffer* buffer );
			void BindIndexBuffer(const GraphicBuffer*indexBuffer);


		protected:
			void BindVertexStream(const GraphicBuffer* buffer, const VertexDeclaration& vertexDec);
			void DoBindFrame(FrameBuffer* fb);
		};
	}
}


#endif // OpenGLRenderDevice_h__