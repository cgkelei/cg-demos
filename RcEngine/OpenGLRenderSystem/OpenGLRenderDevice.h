#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include "Graphics/RenderDevice.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace Render
	{
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

			void AdjustProjectionMatrix(Math::Matrix4f& pOut);
			void Draw(RenderEffect* effect, const RenderOperation& operation);

			void BindVertexBuffer(const GraphicBuffer* buffer );
			void BindIndexBuffer(const GraphicBuffer*indexBuffer);


		protected:
			void DoBindFrameBuffer(FrameBuffer* fb);
			

		private:
			void InitGlew();
			void BindVertexStream(const GraphicBuffer* buffer, const VertexDeclaration& vertexDec);

		private:
			
		};
	}
}


#endif // OpenGLRenderDevice_h__