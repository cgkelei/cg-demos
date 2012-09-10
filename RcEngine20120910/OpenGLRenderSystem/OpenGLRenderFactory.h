#pragma once
#ifndef OpenGLRenderFactory_h__
#define OpenGLRenderFactory_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/RenderFactory.h>


namespace RcEngine {


class _OpenGLExport OpenGLRenderFactory : public RenderFactory
{
public:
	OpenGLRenderFactory(void);
	~OpenGLRenderFactory(void);

	// Texture
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<Texture> CreateTexture1D(uint32_t width, PixelFormat format, uint32_t arrSize, 
		uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, 
		ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat format, 
		uint32_t arrSize, uint32_t numMipMaps,  uint32_t sampleCount, uint32_t sampleQuality, 
		uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTexture3D(uint32_t width, uint32_t height, uint32_t depth, 
		PixelFormat format, uint32_t arraySize, uint32_t numMipMaps,  uint32_t sampleCount,
		uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTextureCube(uint32_t width, uint32_t height, PixelFormat format,
		uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality,
		uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, uint32_t accessHint);


	// Views
	//-------------------------------------------------------------------------------------------------------
	virtual RenderView* Create2DRenderTargetView(Texture* texture, uint32_t arraySize, uint32_t level);

	virtual RenderView* CreateDepthStencilView(Texture* texture, uint32_t arraySize, uint32_t level);

			

	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData);
	

	virtual shared_ptr<Effect> CreateEffectFromFile(const String& effectName, const String& effectFile);

	//virtual RenderEffect*  CreateEffectFromFile(const std::string& effectName, const std::string& effectFile);
private:

};

}

#endif // OpenGLRenderFactory_h__