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
			OpenGLRenderDevice(const RenderSettings& rs);
			~OpenGLRenderDevice(void);

			void AdjustProjectionMatrix(Math::Matrix4f& pOut);
			void Draw(RenderEffect* effect, const RenderOperation& operation);

			void BindVertexBuffer(const GraphicBuffer* buffer );
			void BindIndexBuffer(const GraphicBuffer* indexBuffer);

		protected:
			void BindVertexStream(const GraphicBuffer* buffer, const VertexDeclaration& vertexDec);
			void DoBindFrame(FrameBuffer* fb);
		};
	}
}


#endif // OpenGLRenderDevice_h__