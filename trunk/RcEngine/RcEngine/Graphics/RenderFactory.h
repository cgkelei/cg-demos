#pragma once
#ifndef RenderFactory_h__
#define RenderFactory_h__

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
			virtual shared_ptr<Texture> CreateTexture1D(uint32_t width, PixelFormat format, uint32_t arrSize, 
				uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, 
				ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat format, 
				uint32_t arrSize, uint32_t numMipMaps,  uint32_t sampleCount, uint32_t sampleQuality, 
				uint32_t accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTexture3D(uint32_t width, uint32_t height, uint32_t depth, 
				PixelFormat format, uint32_t arraySize, uint32_t numMipMaps,  uint32_t sampleCount,
				uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTextureCube(uint32_t width, uint32_t height, PixelFormat format,
				uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality,
				uint32_t accessHint, ElementInitData* initData) = 0;

			virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, uint32_t accessHint) = 0;


			// Buffers
			//-------------------------------------------------------------------------------------------------------
			virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;
			virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;


			// Views
			//-------------------------------------------------------------------------------------------------------
			virtual RenderView* Create2DRenderTargetView(Texture* texture, uint32_t arraySize, uint32_t level) = 0;
			virtual RenderView* CreateDepthStencilView(Texture* texture, uint32_t arraySize, uint32_t level) = 0;

			shared_ptr<VertexDeclaration> CreateVertexDeclaration(VertexDeclarationDesc* inputElementDescs, uint32_t numElements);
			shared_ptr<VertexDeclaration> CreateVertexDeclaration(std::vector<VertexElement> elems);
			
			/**
			 * Create a material from file, if the material is already loaded, use the loaded one.
			 * @param matName: material name which is uniquely identify the material.
			 * @param path: the material file location.
			 */
			shared_ptr<Material> CreateMaterialFromFile(const String& matName, const String& path);

			/**
			 * Create a effect from file, if the effect is already loaded, use the loaded one.
			 * @param effectName: effect name which is uniquely identify the effect.
			 * @param effectFile: the effect file location.
			 */
			virtual shared_ptr<Effect> CreateEffectFromFile(const String& effectName, const String& effectFile) = 0;
	

		protected:
			typedef unordered_map<String, shared_ptr<Effect> > EffectMap;
			typedef EffectMap::iterator EffectMapIter;

			typedef unordered_map<String, shared_ptr<Material> > MaterialMap;
			MaterialMap::iterator MaterialMapIter;

			EffectMap mEffectPool;
			MaterialMap mMaterialPool;
		};


	} // RenderSystem
} // RcEngine


#endif // RenderFactory_h__