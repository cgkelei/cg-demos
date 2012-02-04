#include "OpenGLRenderFactory.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLRenderEffect.h"
#include "OpenGLRenderView.h"
#include "OpenGLTexture.h"
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


		shared_ptr<Texture> OpenGLRenderFactory::CreateTexture1D( unsigned int width, PixelFormat format, unsigned int arrSize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLTexture1D>(format, arrSize, numMipMaps, width, sampleCount, sampleQuality, accessHint, initData);
		}

		shared_ptr<Texture> OpenGLRenderFactory::CreateTexture2D( unsigned int width, unsigned int height, PixelFormat format, unsigned int arrSize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLTexture2D>(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
		}

		shared_ptr<Texture> OpenGLRenderFactory::CreateTexture3D( unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLTexture2D>(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
		}

		shared_ptr<Texture> OpenGLRenderFactory::CreateTextureCube( unsigned int width, unsigned int height, PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLTextureCube>(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, initData);
		}

		shared_ptr<GraphicsBuffer> OpenGLRenderFactory::CreateVertexBuffer( BufferUsage usage, uint32 accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLGraphicsBuffer>(usage, accessHint, GL_ARRAY_BUFFER, initData);
		}

		shared_ptr<GraphicsBuffer> OpenGLRenderFactory::CreateIndexBuffer( BufferUsage usage, uint32 accessHint, ElementInitData* initData )
		{
			return std::make_shared<OpenGLGraphicsBuffer>(usage, accessHint, GL_ELEMENT_ARRAY_BUFFER, initData);
		}

		RenderEffect* OpenGLRenderFactory::CreateEffectFromFile( const std::string& effectName, const std::string& effectFile )
		{
			// first check if the file is valid
			/*if ( !fs::exists( effectFile ) || !fs::is_regular_file(effectFile) || fs::file_size(effectFile) == 0 )
			{
				std::cerr << "EffectManager::CreateEffectFromFile: Invalid file: \"" << effectFile << "\"" << std::endl;
				return Effect::InvalidEffect;
			}*/

			//Effect* pEffect = NULL;

			// Look for the effect with the same name in the effect map if it already exists
			EffectMap::iterator effectIter = mEffectPool.find( effectName );
			if(effectIter != mEffectPool.end())
				return effectIter->second;
	    
			//Load a new effect.
			OpenGLRenderEffect* pEffect = new OpenGLRenderEffect( effectFile, effectName );
			mEffectPool.insert( EffectMap::value_type( effectName, pEffect ) );
			return pEffect;
		}


		shared_ptr<Texture> OpenGLRenderFactory::CreateTextureFromFile( const std::string& texFileName, unsigned int accessHint )
		{
			TextureType type;

			nv::Image image;
			image.loadImageFromFile(texFileName.c_str());
	
			int numMipmaps = image.getMipLevels();
			int imageWidth = image.getWidth();
			int imageHeight = image.getHeight();
			int imageDepth = image.getDepth();
			int numCubeFaces = image.getFaces();

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

			unsigned int elementSize;
			elementSize = isCubeMap ? (6*numMipmaps) : numMipmaps;
			std::vector<ElementInitData> imageData(elementSize);

			if(!isCubeMap)
			{
				for (int level = 0; level < numMipmaps; ++level)
				{
					imageData[level].pData = image.getLevel(level);
					imageData[level].rowPitch = image.getImageSize(level);
					imageData[level].slicePitch = 0;
				}
			}
			else
			{
				// cube map
				for (int level = 0; level < numMipmaps; ++level)
				{
					for(unsigned int face = 0; face < 6; ++face)
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


		RenderView* OpenGLRenderFactory::Create2DRenderTargetView( Texture* texture, unsigned int arraySize, unsigned int level )
		{
			return new OpenGLRenderTarget2DView(texture, arraySize, level);
		}

		RenderView* OpenGLRenderFactory::CreateDepthStencilView( Texture* texture, unsigned int arraySize, unsigned int level )
		{
			return 0;
		}

		

		




	}
}
