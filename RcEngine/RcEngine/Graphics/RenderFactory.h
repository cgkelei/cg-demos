#pragma once
#ifndef RenderFactory_h__
#define RenderFactory_h__

#include "Graphics/RenderEffect.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"

namespace RcEngine {
	namespace Render {

		class _ApiExport RenderFactory
		{
		public:
			RenderFactory(void);
			~RenderFactory(void);

			virtual Texture* CreateTexture1D(PixelFormat format, unsigned int numMipMaps, unsigned int width,
				unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData) = 0;

			virtual Texture* CreateTexture2D(PixelFormat format, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData) = 0;

			virtual Texture* CreateTexture3D(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int depth, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData) = 0;

			virtual Texture* CreateTextureCube(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int height, 
				unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData) = 0;

			virtual RenderView* Create2DRenderTargetView(Texture* texture, unsigned int arraySize, unsigned int level) = 0;
			virtual RenderView* CreateDepthStencilView(Texture* texture, unsigned int arraySize, unsigned int level) = 0;

			// Helpful Texture Load Functions
			virtual Texture* CreateTextureFromFile(const std::string& texFileName, unsigned int accessHint) = 0;


			virtual GraphicsBuffer* CreateVertexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData) = 0;
			virtual GraphicsBuffer* CreateIndexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData) = 0;

			virtual RenderEffect* CreateEffectFromFile(const std::string& effectName, const std::string& effectFile) = 0;	

		protected:
			typedef std::map<std::string, RenderEffect*> EffectMap;
			EffectMap mEffectPool;
		};


	} // RenderSystem
} // RcEngine


#endif // RenderFactory_h__