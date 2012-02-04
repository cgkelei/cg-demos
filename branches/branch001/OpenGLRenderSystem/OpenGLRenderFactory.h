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

			// Texture
			//-------------------------------------------------------------------------------------------------------
			virtual shared_ptr<Texture> CreateTexture1D(unsigned int width, PixelFormat format, unsigned int arrSize, 
				unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, 
				ElementInitData* initData);

			virtual shared_ptr<Texture> CreateTexture2D(unsigned int width, unsigned int height, PixelFormat format, 
				unsigned int arrSize, unsigned int numMipMaps,  unsigned int sampleCount, unsigned int sampleQuality, 
				unsigned int accessHint, ElementInitData* initData);

			virtual shared_ptr<Texture> CreateTexture3D(unsigned int width, unsigned int height, unsigned int depth, 
				PixelFormat format, unsigned int arraySize, unsigned int numMipMaps,  unsigned int sampleCount,
				unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			virtual shared_ptr<Texture> CreateTextureCube(unsigned int width, unsigned int height, PixelFormat format,
				unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality,
				unsigned int accessHint, ElementInitData* initData);

			virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, unsigned int accessHint);


			// Views
			//-------------------------------------------------------------------------------------------------------
			virtual RenderView* Create2DRenderTargetView(Texture* texture, unsigned int arraySize, unsigned int level);

			virtual RenderView* CreateDepthStencilView(Texture* texture, unsigned int arraySize, unsigned int level);

			

			virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData);

			virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData);
	
			virtual RenderEffect*  CreateEffectFromFile(const std::string& effectName, const std::string& effectFile);
		private:

		};

	}
}

#endif // OpenGLRenderFactory_h__