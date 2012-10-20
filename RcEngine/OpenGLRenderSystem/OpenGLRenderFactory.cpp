#include "OpenGLRenderFactory.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLRenderView.h"
#include "OpenGLTexture.h"
#include "OpenGLSamplerState.h"
#include "OpenGLShader.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFrameBuffer.h"
#include <Core/Exception.h>
#include <Graphics/BlendState.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/RasterizerState.h>

namespace RcEngine {


OpenGLRenderFactory::OpenGLRenderFactory(void)
{
}

OpenGLRenderFactory::~OpenGLRenderFactory(void)
{
}


shared_ptr<Texture> OpenGLRenderFactory::CreateTexture1D( uint32_t width, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLTexture1D>(format, arrSize, numMipMaps, width, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTexture2D( uint32_t width, uint32_t height, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLTexture2D>(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTexture3D( uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLTexture2D>(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTextureCube( uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLTextureCube>(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<GraphicsBuffer> OpenGLRenderFactory::CreateVertexBuffer( BufferUsage usage, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLGraphicsBuffer>(usage, accessHint, GL_ARRAY_BUFFER, initData);
}

shared_ptr<GraphicsBuffer> OpenGLRenderFactory::CreateIndexBuffer( BufferUsage usage, uint32_t accessHint, ElementInitData* initData )
{
	return std::make_shared<OpenGLGraphicsBuffer>(usage, accessHint, GL_ELEMENT_ARRAY_BUFFER, initData);
}

shared_ptr<DepthStencilState> OpenGLRenderFactory::CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc )
{
	return std::make_shared<DepthStencilState>(desc);
}

shared_ptr<BlendState> OpenGLRenderFactory::CreateBlendStateImpl( const BlendStateDesc& desc )
{
	return std::make_shared<BlendState>(desc);
}

shared_ptr<RasterizerState> OpenGLRenderFactory::CreateRasterizerStateImpl( const RasterizerStateDesc& desc )
{
	return std::make_shared<RasterizerState>(desc);
}

shared_ptr<SamplerState> OpenGLRenderFactory::CreateSamplerStateImpl( const SamplerStateDesc& desc )
{
	return std::make_shared<OpenGLSamplerState>(desc);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTextureFromFile( const std::string& texFileName, uint32_t accessHint )
{
	TextureType type;

	nv::Image image;

	if (!image.loadImageFromFile(texFileName.c_str()))
	{
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Load Texture Error", "OpenGLRenderFactory::CreateTextureFromFile");
	}
			
	int32_t numMipmaps = image.getMipLevels();
	int32_t imageWidth = image.getWidth();
	int32_t imageHeight = image.getHeight();
	int32_t imageDepth = image.getDepth();
	int32_t numCubeFaces = image.getFaces();

	bool isCompressed = image.isCompressed();
	bool isCubeMap = image.isCubeMap();
	bool isVolume = image.isVolume();


	if( isCubeMap )
	{
		type = TT_TextureCube;
	}
	else if (isVolume && imageDepth>0)
	{
		type = TT_Texture3D;
	}
	else if(imageHeight>0 && imageWidth>0)
	{
		type = TT_Texture2D;
	}else if(imageWidth>0 && imageHeight==0)
	{
		type = TT_Texture1D;
	}else
	{
		assert(false);
	}


	PixelFormat format;
	format = OpenGLMapping::UnMapping(image.getInternalFormat(), image.getFormat(), image.getType());

	uint32_t elementSize;
	elementSize = isCubeMap ? (6*numMipmaps) : numMipmaps;
	std::vector<ElementInitData> imageData(elementSize);

	if(!isCubeMap)
	{
		for (int32_t level = 0; level < numMipmaps; ++level)
		{
			imageData[level].pData = image.getLevel(level);
			imageData[level].rowPitch = image.getImageSize(level);
			imageData[level].slicePitch = 0;
		}
	}
	else
	{
		// A texture cube is a 2D texture array that contains 6 textures, one for each face of the cube
		for(uint32_t face = 0; face < 6; ++face)
		{
			for (int32_t level = 0; level < numMipmaps; ++level)
			{
				imageData[face*numMipmaps+level].pData = image.getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
				imageData[face*numMipmaps+level].rowPitch = image.getImageSize(level);
				imageData[face*numMipmaps+level].slicePitch = 0;
			}
		}
	}


	switch(type)
	{
	case TT_Texture1D:
		return CreateTexture1D(imageWidth, format, 1, numMipmaps, 0, 1, accessHint, &imageData[0]);
	case TT_Texture2D:
		return CreateTexture2D(imageWidth, imageHeight, format, 1, numMipmaps, 0, 1, accessHint, &imageData[0]);
	case TT_Texture3D:
		return CreateTexture3D(imageWidth, imageHeight, imageDepth, format, 1, numMipmaps, 0, 1, accessHint, &imageData[0]);
	case TT_TextureCube:
		return CreateTextureCube(imageWidth, imageHeight, format, 1, numMipmaps, 0, 1, accessHint, &imageData[0]);
	};
	
	ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Unsupported Texture Format", "OpenGLRenderFactory::CreateTextureFromFile");
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTextureArrayFromFile( const vector<String>& textures, uint32_t accessHint /*= 0*/ )
{
	TextureType textureArrayType;
	uint32_t textureWidth;
	uint32_t textureHeight;
	uint32_t textureMipMaps;
	PixelFormat textureFormat;
	
	nv::Image image;
	
	vector<ElementInitData> imageData;
	
	size_t arraySize = textures.size();
	for (size_t arrIndex = 0; arrIndex < textures.size(); ++arrIndex)
	{
		if (!image.loadImageFromFile(textures[arrIndex].c_str()))
		{
			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Load texture error: " + textures[arrIndex] + "doesn't exit.",
				"OpenGLRenderFactory::CreateTextureFromFile");
		}

		int32_t numMipmaps = image.getMipLevels();
		int32_t imageWidth = image.getWidth();
		int32_t imageHeight = image.getHeight();
		int32_t imageDepth = image.getDepth();
		int32_t numCubeFaces = image.getFaces();

		bool isCompressed = image.isCompressed();
		bool isCubeMap = image.isCubeMap();
		bool isVolume = image.isVolume();
		
		TextureType type;
		if( isCubeMap )
		{
			type = TT_TextureCube;
		}
		else if (isVolume && imageDepth>0)
		{
			type = TT_Texture3D;
		}
		else if(imageHeight>0 && imageWidth>0)
		{
			type = TT_Texture2D;
		}else if(imageWidth>0 && imageHeight==0)
		{
			type = TT_Texture1D;
		}else
		{
			assert(false);
		}

		PixelFormat format;
		format = OpenGLMapping::UnMapping(image.getInternalFormat(), image.getFormat(), image.getType());

		// all texture in texture array must have the same data format and dimensions (including mipmap levels)
		if ( arrIndex == 0)
		{
			textureArrayType = type;
			textureWidth = imageWidth;
			textureHeight = imageHeight;
			textureMipMaps = numMipmaps;
			textureFormat = format;

			uint32_t elementSize = isCubeMap ? (6*numMipmaps*arraySize) : numMipmaps*arraySize;
			imageData.resize(elementSize);
		}
		else
		{
			if (textureArrayType != type || textureWidth != imageWidth ||
				textureHeight != imageHeight || textureMipMaps != numMipmaps || textureFormat != format)
			{
				ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Texture array must created with the same texture type",
					"OpenGLRenderFactory::CreateTextureFromFile");
			}
		}
		
		if(!isCubeMap)
		{
			for (int32_t level = 0; level < numMipmaps; ++level)
			{
				uint32_t imageIndex = arrIndex * textureMipMaps + level;
				imageData[imageIndex].pData = image.getLevel(level);
				imageData[imageIndex].rowPitch = image.getImageSize(level);
				imageData[imageIndex].slicePitch = 0;
			}
		}
		else
		{
			// A texture cube is a 2D texture array that contains 6 textures, one for each face of the cube
			for(uint32_t face = 0; face < 6; ++face)
			{
				for (int32_t level = 0; level < numMipmaps; ++level)
				{
					uint32_t imageIndex =  arrIndex*textureMipMaps*6 + face*textureMipMaps + level;
					imageData[imageIndex].pData = image.getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
					imageData[imageIndex].rowPitch = image.getImageSize(level);
					imageData[imageIndex].slicePitch = 0;
				}
			}
		}
	}

	switch(textureArrayType)
	{
	case TT_Texture1D:
		return CreateTexture1D(textureWidth, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
	case TT_Texture2D:
		return CreateTexture2D(textureWidth, textureHeight, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
	case TT_TextureCube:
		return CreateTextureCube(textureWidth, textureHeight, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
	default:
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Unsupported Texture Format", "OpenGLRenderFactory::CreateTextureFromFile");
	};
}


shared_ptr<RenderView> OpenGLRenderFactory::CreateDepthStencilView( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return std::make_shared<OpenGLDepthStencilView>(*texture, arrayIndex, level);
}

shared_ptr<RenderView> OpenGLRenderFactory::CreateRenderTargetView2D( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return std::make_shared<OpenGLRenderTargetView2D>(*texture, arrayIndex, level);
}

shared_ptr<Shader> OpenGLRenderFactory::CreateShader( ShaderType type )
{
	return std::make_shared<OpenGLShader>(type);
}

shared_ptr<ShaderProgram> OpenGLRenderFactory::CreateShaderProgram( Effect& effect )
{
	return std::make_shared<OpenGLShaderProgram>(effect);
}

shared_ptr<FrameBuffer> OpenGLRenderFactory::CreateFrameBuffer( uint32_t width, uint32_t height )
{
	return std::make_shared<OpenGLFrameBuffer>(width, height, true);
}




}
