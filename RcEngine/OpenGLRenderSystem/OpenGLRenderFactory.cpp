#include "OpenGLRenderFactory.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLRenderView.h"
#include "OpenGLTexture.h"
#include "OpenGLEffect.h"
#include "Core/Exception.h"


namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

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

		//RenderEffect* OpenGLRenderFactory::CreateEffectFromFile( const std::string& effectName, const std::string& effectFile )
		//{
		//	// first check if the file is valid
		//	/*if ( !fs::exists( effectFile ) || !fs::is_regular_file(effectFile) || fs::file_size(effectFile) == 0 )
		//	{
		//		std::cerr << "EffectManager::CreateEffectFromFile: Invalid file: \"" << effectFile << "\"" << std::endl;
		//		return Effect::InvalidEffect;
		//	}*/

		//	//Effect* pEffect = NULL;

		//	// Look for the effect with the same name in the effect map if it already exists
		//	EffectMap::iterator effectIter = mEffectPool.find( effectName );
		//	if(effectIter != mEffectPool.end())
		//		return effectIter->second;
	 //   
		//	//Load a new effect.
		//	OpenGLRenderEffect* pEffect = new OpenGLRenderEffect( effectFile, effectName );
		//	mEffectPool.insert( EffectMap::value_type( effectName, pEffect ) );
		//	return pEffect;
		//}


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
				// cube map
				for (int32_t level = 0; level < numMipmaps; ++level)
				{
					for(uint32_t face = 0; face < 6; ++face)
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


		RenderView* OpenGLRenderFactory::Create2DRenderTargetView( Texture* texture, uint32_t arraySize, uint32_t level )
		{
			return new OpenGLRenderTarget2DView(texture, arraySize, level);
		}

		RenderView* OpenGLRenderFactory::CreateDepthStencilView( Texture* texture, uint32_t arraySize, uint32_t level )
		{
			return 0;
		}

		shared_ptr<Effect> OpenGLRenderFactory::CreateEffectFromFile( const String& effectName, const String& effectFile )
		{
			return OpenGLEffect::LoadEffectFromFile(effectFile, effectName);
		}

		

		




	}
}
