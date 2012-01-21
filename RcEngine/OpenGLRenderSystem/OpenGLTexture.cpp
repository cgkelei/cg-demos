#include "OpenGLTexture.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLTexture::OpenGLTexture( TextureType type, PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint )
			: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			mTextureArraySize = arraySize;

			switch(mType)
			{
			case TT_Texture1D:
				mTargetType =  (mTextureArraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
				break;
			case TT_Texture2D:
				mTargetType =  (mTextureArraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
				break;
			case  TT_Texture3D:
				mTargetType = GL_TEXTURE_3D;
				break;
			case  TT_TextureCube:
				mTargetType = (mTextureArraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;

				break;
			default:
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture type not supported!", "OpenGLTexture::OpenGLTexture");
			}
		}


		OpenGLTexture::~OpenGLTexture(void)
		{
		}

		unsigned int OpenGLTexture::GetWidth( unsigned int level )
		{
			assert(level < mMipMaps);
			return 1;
		}

		unsigned int OpenGLTexture::GetHeight( unsigned int level )
		{
			assert(level < mMipMaps);
			return 1;
		}

		unsigned int OpenGLTexture::GetDepth( unsigned int level )
		{
			assert(level < mMipMaps);
			return 1;
		}

		void OpenGLTexture::Map1D(unsigned int arrayIndex,  unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int width, void*& data )
		{

		}

		void OpenGLTexture::Map2D(unsigned int arrayIndex,  unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, void*& data, unsigned int& rowPitch )
		{

		}

		void OpenGLTexture::Map3D(unsigned int arrayIndex,  unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int zOffset, unsigned int width, unsigned int height, unsigned int depth, void*& data, unsigned int& rowPitch, unsigned int& slicePitch )
		{

		}

		void OpenGLTexture::MapCube(unsigned int arrayIndex,  CubeMapFace face, unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, void*& data, unsigned int& rowPitch )
		{

		}

		void OpenGLTexture::Unmap1D(unsigned int arrayIndex,   unsigned int level )
		{

		}

		void OpenGLTexture::Unmap2D(unsigned int arrayIndex,  unsigned int level )
		{

		}

		void OpenGLTexture::Unmap3D(unsigned int arrayIndex,  unsigned int level )
		{

		}

		void OpenGLTexture::UnmapCube(unsigned int arrayIndex,   CubeMapFace face, unsigned int level )
		{

		}
	}
}





