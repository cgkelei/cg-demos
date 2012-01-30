#pragma once
#ifndef OpenGLRenderFactory_h__
#define OpenGLRenderFactory_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/RenderFactory.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace Render
	{

		class _OpenGLExport OpenGLRenderFactory : public RenderFactory
		{
		public:
			OpenGLRenderFactory(void);
			~OpenGLRenderFactory(void);

			virtual Texture* CreateTexture1D(PixelFormat format, unsigned int numMipMaps, unsigned int width,
				unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			virtual Texture* CreateTexture2D(PixelFormat format, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			virtual Texture* CreateTexture3D(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int depth, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			virtual Texture* CreateTextureCube(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int height, 
				unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);
	
			virtual RenderView* Create2DRenderTargetView(Texture* texture, unsigned int arraySize, unsigned int level);

			virtual RenderView* CreateDepthStencilView(Texture* texture, unsigned int arraySize, unsigned int level);

			virtual Texture* CreateTextureFromFile(const std::string& texFileName, unsigned int accessHint);

			virtual GraphicsBuffer* CreateVertexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData);

			virtual GraphicsBuffer* CreateIndexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData);
	
			virtual RenderEffect*  CreateEffectFromFile(const std::string& effectName, const std::string& effectFile);
		private:

		};

	}
}

#endif // OpenGLRenderFactory_h__