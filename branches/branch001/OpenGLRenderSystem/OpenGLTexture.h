#ifndef OpenGLTexture_h__
#define OpenGLTexture_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include "Graphics/Texture.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLTexture : public Texture
		{
		public:
			OpenGLTexture(TextureType type, PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint );
			virtual ~OpenGLTexture(void);

			virtual unsigned int GetWidth(unsigned int level);
			virtual unsigned int GetHeight(unsigned int level);
			virtual unsigned int GetDepth(unsigned int level); 

			virtual void Map1D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int width, void*& data);

			virtual void Map2D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
				void*& data, unsigned int& rowPitch);

			virtual void Map3D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int zOffset,
				unsigned int width, unsigned int height, unsigned int depth,
				void*& data, unsigned int& rowPitch, unsigned int& slicePitch);

			virtual void MapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
				void*& data, unsigned int& rowPitch);

			virtual void Unmap1D(unsigned int arrayIndex, unsigned int level);
			virtual void Unmap2D(unsigned int arrayIndex, unsigned int level);
			virtual void Unmap3D(unsigned int arrayIndex, unsigned int level);
			virtual void UnmapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level);

			GLuint GetOpenGLTexture() const { return mTextureID; }
			GLenum GetOpenGLTextureTarget() const { return mTargetType; }

		protected:
			GLuint mTextureID;
			GLenum mTargetType;
			std::vector<GLuint> mPixelBuffers;
			std::vector< std::vector<unsigned char> > mTextureData;
		};


		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLTexture1D : public OpenGLTexture
		{
		public:
			OpenGLTexture1D(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			~OpenGLTexture1D();

			virtual unsigned int GetWidth(unsigned int level);

			virtual void Map1D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int width, void*& data);

			virtual void Unmap1D(unsigned int arrayIndex, unsigned int level);
		private:
			std::vector<unsigned int> mWidths;

		};


		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLTexture2D : public OpenGLTexture
		{
		public:
			OpenGLTexture2D(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			~OpenGLTexture2D();

			virtual unsigned int GetWidth(unsigned int level);
			virtual unsigned int GetHeight(unsigned int level);

			virtual void Map2D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
				void*& data, unsigned int& rowPitch);

			virtual void Unmap2D(unsigned int arrayIndex, unsigned int level);

		private:
			std::vector<unsigned int> mWidths;
			std::vector<unsigned int> mHeights;

		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLTexture3D : public OpenGLTexture
		{
		public:
			OpenGLTexture3D(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int depth, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);
			~OpenGLTexture3D();

			virtual unsigned int GetWidth(unsigned int level);
			virtual unsigned int GetHeight(unsigned int level);
			virtual unsigned int GetDepth(unsigned int level);

			virtual void Map3D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int zOffset,
				unsigned int width, unsigned int height, unsigned int depth,
				void*& data, unsigned int& rowPitch, unsigned int& slicePitch);

			virtual void Unmap3D(unsigned int arrayIndex, unsigned int level);


		private:
			std::vector<unsigned int> mWidths;
			std::vector<unsigned int> mHeights;
			std::vector<unsigned int> mDepths;

		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLTextureCube : public OpenGLTexture
		{
		public:
			OpenGLTextureCube(PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width,
				unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData);

			~OpenGLTextureCube();

			virtual unsigned int GetWidth(unsigned int level);
			virtual unsigned int GetHeight(unsigned int level);

			virtual void MapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level, TextureMapAccess tma,
				unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
				void*& data, unsigned int& rowPitch);

			virtual void UnmapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level);

		private:
			std::vector<unsigned int> mSizes;

		};

	}
}

#endif // OpenGLTexture_h__
