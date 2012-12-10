#include "Utility.h"
#include "pfm.h"
#include <cstdint>
#include <fstream>
#include <vector>
#include <nvSDKPath.h>
#include <nvShaderUtils.h>

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

void Utility::SaveTextureToPfmDepth( const char* file, GLuint tex, int width, int height )
{
	std::vector<float> TextureData(width * height);

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &TextureData[0]);

	WritePfm(file, width, height, 1, &TextureData[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Utility::SaveTextureToPfm( const char* file, GLuint tex, int width, int height )
{
	std::vector<float> TextureData(width * height * 3);

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &TextureData[0]);

	WritePfm(file, width, height, 3, &TextureData[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Utility::SaveTextureToTGA( const char* file, GLuint tex, int width, int height )
{
	std::vector<uint8_t> TextureData(width * height * 4);

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, &TextureData[0]);

	uint8_t* pixel = (uint8_t*)&TextureData[0];
	std::vector<Pixel32> imageData(width*height);

	auto h = height;
	auto w = width;

	for (int32_t j = 0; j < h; j++)
		for(int32_t i = 0; i < w; i ++)
		{
			uint8_t b = pixel[((h - 1 - j) * w + i)*4];
			uint8_t g = pixel[((h - 1 - j) * w + i)*4 + 1];
			uint8_t r = pixel[((h - 1 - j) * w + i)*4 + 2];
			uint8_t a = pixel[((h - 1 - j) * w + i)*4 + 3];

			imageData[j*w+i].r = r;
			imageData[j*w+i].g = g;
			imageData[j*w+i].b = b;
			imageData[j*w+i].a = a;
		}

		WriteTGA(file, &imageData[0], w, h);
}

void Utility::PrintEffectUniforms( GLuint mOGLProgramObject )
{
	// Query and store uniforms from the program.
	GLint activeUniforms;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0)
	{
		GLint maxLength;
		glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
		std::vector<char> maxName(maxLength);

		GLint length;
		GLint uniformSize;
		GLenum uniformType;
		unsigned int samplerIndex = 0;	

		for (GLint i = 0; i < activeUniforms; ++i)
		{
			glGetActiveUniform(mOGLProgramObject, i, maxLength, &length, &uniformSize, &uniformType, &maxName[0]);

			bool isArray = false;
			if (uniformSize > 1 && maxLength > 3)
			{
				// This is an array uniform
				isArray = true;
				GLint lengthWithoutBracket = 0;
				while( lengthWithoutBracket < length && maxName[lengthWithoutBracket] != '[') lengthWithoutBracket++;
				length = lengthWithoutBracket;
			}

			std::string actualName(&maxName[0], length);
			GLint location = glGetUniformLocation(mOGLProgramObject, actualName.c_str());

			if (isArray)
			{
				bool end = false;
				int i = 0;
				while (!end)
				{
					std::stringstream sss; 
					sss  << actualName << "[" << i << "]";
					auto str = sss.str();
					GLint location = glGetUniformLocation(mOGLProgramObject, sss.str().c_str());

					if(location >= 0)
					{
						std::cout << "Location: " << location << "   Name: " << str << std::endl; 
						i++;
					}
					else
					{
						end = true;
					}

				}
			}
			else
			{
				std::cout << "Location: " << location << "   Name: " << actualName << std::endl; 
			}
		}
	}		
}

extern nv::SDKPath gAppPath;

GLuint Utility::LoadShaderEffect( const char* vsFile, const char* fsFile )
{
	std::string resolvedPath;
	GLuint shaderVS, shaderFS;

	if (gAppPath.getFilePath( vsFile, resolvedPath)) 
	{
		shaderVS = nv::CompileGLSLShaderFromFile(GL_VERTEX_SHADER, resolvedPath.c_str());
	} else
	{
		fprintf(stderr, "Failed to locate program '%s'\n", vsFile);
		return 0;
	}

	if (gAppPath.getFilePath( fsFile, resolvedPath)) 
	{
		shaderFS = nv::CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER, resolvedPath.c_str());
	} else
	{
		fprintf(stderr, "Failed to locate program '%s'\n", fsFile);
		return 0;
	}

	/*ASSERT(shaderVS > 0);
	shaderVS = nv::CompileGLSLShaderFromFile(GL_VERTEX_SHADER, vsFile);
	shaderFS = nv::CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER, fsFile);
	ASSERT(shaderFS > 0);*/

	return nv::LinkGLSLProgram(shaderVS, shaderFS);
}

void Utility::PrintEffectAttribs( GLuint program )
{
	GLint activeAttribs;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
	if (activeAttribs > 0)
	{
		GLint maxLength;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
		std::vector<char> maxName(maxLength);

		GLint length;
		GLint attibSize;
		GLenum attibType;
		unsigned int samplerIndex = 0;	

		for (GLint i = 0; i < activeAttribs; ++i)
		{
			glGetActiveAttrib(program, i, maxLength, &length, &attibSize, &attibType, &maxName[0]);
			std::string actualName(&maxName[0], length);

			
			std::cout << glGetAttribLocation(program, actualName.c_str()) << " " << actualName << std::endl;
		}
	}
}
