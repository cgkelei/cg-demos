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
#include "pfm.h"

namespace {

union Pixel32
{
	Pixel32()
		: integer(0) { }
	Pixel32(uint8_t bi, uint8_t gi, uint8_t ri, uint8_t ai = 255)
	{
		b = bi;
		g = gi;
		r = ri;
		a = ai;
	}

	uint8_t integer;

	struct
	{
#ifdef BIG_ENDIAN
		uint8_t a, r, g, b;
#else // BIG_ENDIAN
		uint8_t b, g, r, a;
#endif // BIG_ENDIAN
	};
};

// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
	uint8_t   idLength,           // Length of optional identification sequence.
		paletteType,        // Is a palette present? (1=yes)
		imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
	// 3=grey, +8=rle packed).
	uint16_t  firstPaletteEntry,  // First palette index, if present.
		numPaletteEntries;  // Number of palette entries, if present.
	uint8_t   paletteBits;        // Number of bits per palette entry.
	uint16_t  x,                  // Horiz. pixel coord. of lower left of image.
		y,                  // Vert. pixel coord. of lower left of image.
		width,              // Image width in pixels.
		height;             // Image height in pixels.
	uint8_t   depth,              // Image color depth (bits per pixel).
		descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

bool
	WriteTGA(const std::string &filename,
	const Pixel32 *pxl,
	uint16_t width,
	uint16_t height)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		TGAHeader header;
		memset(&header, 0, sizeof(TGAHeader));
		header.imageType  = 2;
		header.width = width;
		header.height = height;
		header.depth = 32;
		header.descriptor = 0x20;

		file.write((const char *)&header, sizeof(TGAHeader));
		file.write((const char *)pxl, sizeof(Pixel32) * width * height);

		return true;
	}
	return false;
}





}


namespace RcEngine {


OpenGLRenderFactory::OpenGLRenderFactory(void)
{
}

OpenGLRenderFactory::~OpenGLRenderFactory(void)
{
}


shared_ptr<Texture> OpenGLRenderFactory::CreateTexture1D( uint32_t width, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	shared_ptr<OpenGLTexture1D> retVal(
		new OpenGLTexture1D(format, arrSize, numMipMaps, width, sampleCount, sampleQuality, accessHint, initData));

	return retVal;

	// VS2012 Bug
	//return std::make_shared<OpenGLTexture1D>(format, arrSize, numMipMaps, width, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTexture2D( uint32_t width, uint32_t height, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	shared_ptr<OpenGLTexture2D> retVal(
		new OpenGLTexture2D(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData));

	return retVal;
	//return std::make_shared<OpenGLTexture2D>(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTexture3D( uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	shared_ptr<OpenGLTexture3D> retVal(
		new OpenGLTexture3D(format, arraySize, numMipMaps, width, height, depth, sampleCount, sampleQuality, accessHint, initData));
	
	return retVal;
	//return std::make_shared<OpenGLTexture3D>(format, arraySize, numMipMaps, width, height, depth, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLRenderFactory::CreateTextureCube( uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
{
	shared_ptr<OpenGLTextureCube> retVal(
		new OpenGLTextureCube(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData));

	return retVal;//return std::make_shared<OpenGLTextureCube>(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
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
		return CreateTexture1D(imageWidth, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
	case TT_Texture2D:
		return CreateTexture2D(imageWidth, imageHeight, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
	case TT_Texture3D:
		return CreateTexture3D(imageWidth, imageHeight, imageDepth, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
	case TT_TextureCube:
		return CreateTextureCube(imageWidth, imageHeight, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
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

void ExportToPfm(const String& filename, uint32_t width, uint32_t height, PixelFormat format, void* data)
{
	vector<float> temp;
	temp.resize(width * height * 3);
	float* temPixel = &temp[0];

	uint8_t* pixel = (uint8_t*)data;
	
	if (format == PF_A8R8G8B8)
	{
		for (uint32_t j = 0; j < height; j++)
			for(uint32_t i = 0; i < width; i ++)
		{	
			uint8_t b = pixel[(j * width + i)*4 + 0];
			uint8_t g = pixel[(j * width + i)*4 +1];
			uint8_t r = pixel[(j * width + i)*4 +2];
			uint8_t a = pixel[(j * width + i)*4 +3];

			assert(a == 255);
			temPixel[0] = r / 255.0f;
			temPixel[1] = g / 255.0f;
			temPixel[2] = b / 255.0f;
			temPixel += 3;
		}

			FILE* file = fopen(filename.c_str(), "wb");
			assert(file != NULL);
			fprintf(file, "PF\n%d %d\n-1.0\n", width, height);
			fwrite(&temp[0],sizeof(float) * temp.size(), 1,file);
			fclose(file);
	}
	else if (format == PF_Luminance8)
	{
		for (uint32_t j = 0; j < height; j++)
			for(uint32_t i = 0; i < width; i ++)
			{	
				uint8_t b = pixel[(j * width + i)];
				uint8_t g = pixel[(j * width + i)];
				uint8_t r = pixel[(j * width + i)];
				uint8_t a = pixel[(j * width + i)];

				temPixel[0] = r / 255.0f;
				temPixel[1] = g / 255.0f;
				temPixel[2] = b / 255.0f;
				temPixel += 3;
			}

			FILE* file = fopen(filename.c_str(), "wb");
			assert(file != NULL);
			fprintf(file, "PF\n%d %d\n-1.0\n", width, height);
			fwrite(&temp[0],sizeof(float) * temp.size(), 1,file);
			fclose(file);
	}
}

void OpenGLRenderFactory::SaveTexture2D( const String& texFile, const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level)
{
	assert(texture->GetTextureType() == TT_Texture2D);

	uint32_t w = texture->GetWidth(level);
	uint32_t h = texture->GetHeight(level);

	if (texture->GetTextureFormat() == PF_A8R8G8B8)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		uint8_t* pixel = (uint8_t*)pData;
		vector<Pixel32> imageData(w*h);
		for (uint32_t j = 0; j < h; j++)
			for(uint32_t i = 0; i < w; i ++)
			{
				uint8_t b = pixel[((h-j -1) * w + i)*4 + 0];
				uint8_t g = pixel[((h-j-1) * w + i)*4 +1];
				uint8_t r = pixel[((h-j-1) * w + i)*4 +2];
				uint8_t a = pixel[((h-j-1) * w + i)*4 +3];

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = a;
			}

			WriteTGA(texFile.c_str(), &imageData[0], w, h);
	}
	else if (texture->GetTextureFormat() == PF_A8B8G8R8)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		uint8_t* pixel = (uint8_t*)pData;
		vector<Pixel32> imageData(w*h);
		for (uint32_t j = 0; j < h; j++)
			for(uint32_t i = 0; i < w; i ++)
			{
				uint8_t r = pixel[((h-j -1) * w + i)*4 + 0];
				uint8_t g = pixel[((h-j-1) * w + i)*4 +1];
				uint8_t b = pixel[((h-j-1) * w + i)*4 +2];
				uint8_t a = pixel[((h-j-1) * w + i)*4 +3];

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = a;
			}

			WriteTGA(texFile.c_str(), &imageData[0], w, h);
	}
	else if (texture->GetTextureFormat() == PF_R8G8B8)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		uint8_t* pixel = (uint8_t*)pData;
		vector<Pixel32> imageData(w*h);
		for (uint32_t j = 0; j < h; j++)
			for(uint32_t i = 0; i < w; i ++)
			{
				uint8_t b = pixel[((h-j -1) * w + i)*3 + 0];
				uint8_t g = pixel[((h-j-1) * w + i)*3 +1];
				uint8_t r = pixel[((h-j-1) * w + i)*3 +2];

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = 255;
			}

			WriteTGA(texFile.c_str(), &imageData[0], w, h);
	}
	else if (texture->GetTextureFormat() == PF_Alpha8)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		uint8_t* pixel = (uint8_t*)pData;
		vector<Pixel32> imageData(w*h);
		for (uint32_t j = 0; j < h; j++)
			for(uint32_t i = 0; i < w; i ++)
			{
				uint8_t b = pixel[((j) * w + i)];
				uint8_t g = pixel[((j) * w + i)];
				uint8_t r = pixel[((j) * w + i)];
				uint8_t a = 255;

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = a;
			}

			WriteTGA(texFile.c_str(), &imageData[0], w, h);
	}
	else if (texture->GetTextureFormat() == PF_A32B32G32R32F)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		float* pixel = (float*)pData;
		/*vector<Pixel32> imageData(w*h);
		for (uint32_t j = 0; j < h; j++)
		for(uint32_t i = 0; i < w; i ++)
		{
		float r = pixel[((h-j -1) * w + i)*4 + 0];
		float g = pixel[((h-j-1) * w + i)*4 +1];
		float b = pixel[((h-j-1) * w + i)*4 +2];
		float a = pixel[((h-j-1) * w + i)*4 +3];

		imageData[j*w+i].r = r*255;
		imageData[j*w+i].g = g*255;
		imageData[j*w+i].b = b*255;
		imageData[j*w+i].a = a*255;
		}*/

		//WriteTGA(texFile.c_str(), &imageData[0], w, h);

		vector<float> temp;
		temp.resize(w * h * 3);
		float* imageData = &temp[0];
		for (uint32_t j = 0; j < h; j++)
		for(uint32_t i = 0; i < w; i ++)
		{
			float r = pixel[(j * w + i)*4 + 0];
			float g = pixel[(j * w + i)*4 +1];
			float b = pixel[(j * w + i)*4 +2];
			float a = pixel[(j * w + i)*4 +3];

			*imageData++ = r;
			*imageData++ = g;
			*imageData++ = b;
		}

		WritePfm(texFile.c_str(), w, h, 3, &temp[0]);
		
	}
	else if (texture->GetTextureFormat() == PF_Depth32)
	{
		void* pData;
		uint32_t rowPitch;
		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);

		float* pixel = (float*)pData;

		vector<float> temp;
		temp.resize(w * h);
		float* imageData = &temp[0];
		for (uint32_t j = 0; j < h; j++)
		for(uint32_t i = 0; i < w; i ++)
		{
			float r = pixel[(j * w + i)];
			*imageData++ = r;
		}

		WritePfm(texFile.c_str(), w, h, 1, &temp[0]);
	}
	else if (texture->GetTextureFormat() == PF_Depth24Stencil8)
	{		
		shared_ptr<OpenGLTexture> oglTexture = std::static_pointer_cast<OpenGLTexture>(texture);

		std::vector<uint8_t> data(w*h);
		glBindTexture(oglTexture->GetOpenGLTextureTarget(), oglTexture->GetOpenGLTexture());
		glGetTexImage(oglTexture->GetOpenGLTextureTarget(), 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &data[0]);
		GLenum err = glGetError();
	}
}

}


