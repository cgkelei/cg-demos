#pragma once
#ifndef RenderFactory_h__
#define RenderFactory_h__

#include "Graphics/RenderEffect.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"
#include "VertexDeclaration.h"

namespace RcEngine {
	namespace Render {

		class _ApiExport RenderFactory
		{
		public:
			RenderFactory(void);
			~RenderFactory(void);

			// Texture
			//-------------------------------------------------------------------------------------------------------
			virtual shared_ptr<Texture> CreateTexture1D(unsigned int width, PixelFormat format, unsigned int arrSize, 
				unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, 
				ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTexture2D(unsigned int width, unsigned int height, PixelFormat format, 
				unsigned int arrSize, unsigned int numMipMaps,  unsigned int sampleCount, unsigned int sampleQuality, 
				unsigned int accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTexture3D(unsigned int width, unsigned int height, unsigned int depth, 
				PixelFormat format, unsigned int arraySize, unsigned int numMipMaps,  unsigned int sampleCount,
				unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTextureCube(unsigned int width, unsigned int height, PixelFormat format,
				unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality,
				unsigned int accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, unsigned int accessHint) = 0;


			// Buffers
			//-------------------------------------------------------------------------------------------------------
			virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData) = 0;
			virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32 accessHint, ElementInitData* initData) = 0;


			// Views
			//-------------------------------------------------------------------------------------------------------
			virtual RenderView* Create2DRenderTargetView(Texture* texture, unsigned int arraySize, unsigned int level) = 0;
			virtual RenderView* CreateDepthStencilView(Texture* texture, unsigned int arraySize, unsigned int level) = 0;

			shared_ptr<VertexDeclaration> CreateVertexDeclaration(VertexDeclarationDesc* inputElementDescs, unsigned int numElements);
			


			

			virtual RenderEffect* CreateEffectFromFile(const std::string& effectName, const std::string& effectFile) = 0;	

		protected:
			typedef std::map<std::string, RenderEffect*> EffectMap;
			EffectMap mEffectPool;
		};


	} // RenderSystem
} // RcEngine


#endif // RenderFactory_h__