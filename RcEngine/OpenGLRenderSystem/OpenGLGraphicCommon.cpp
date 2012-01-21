#include "OpenGLGraphicCommon.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		using Core::Exception;

		OpenGLMapping::OpenGLMapping(void)
		{
		}


		OpenGLMapping::~OpenGLMapping(void)
		{
		}

		GLenum OpenGLMapping::Mapping( VertexElementFormat format )
		{
			switch(format)
			{
			case VEF_Single:
			case VEF_Vector2:
			case VEF_Vector3:
			case VEF_Vector4:
				return GL_FLOAT;


			case VEF_Short2:
			case VEF_Short4:
			case VEF_NormalizedShort2:
			case VEF_NormalizedShort4:
				return GL_SHORT;


			case VEF_UByte4:
				return GL_UNSIGNED_BYTE;


			case VEF_HalfVector2:
			case VEF_HalfVector4:
				return GL_FLOAT;

			}
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported vertex format", "OpenGLGraphicCommon::Mapping");
		}

		GLenum OpenGLMapping::Mapping( PrimitiveType type )
		{
			switch(type)
			{
			case PT_Point_List:
				return GL_POINTS;
			case PT_Line_List:
				return GL_LINES;
			case PT_Line_Strip:
				return GL_LINE_STRIP;
			case PT_Triangle_List:
				return GL_TRIANGLES;
			case PT_Triangle_Strip:
				return GL_TRIANGLE_STRIP;
			case PT_Triangle_Fan:
				return GL_TRIANGLE_FAN;
			}
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported PrimitiveType", "OpenGLGraphicCommon::Mapping");
		}

		void OpenGLMapping::Mapping( GLint& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat )
		{
			switch(inPixelFormat)
			{
			case PF_Luminance8:
				outInternalformat = GL_LUMINANCE8;
				outFormat = GL_LUMINANCE8;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_Luminance16:
				outInternalformat = GL_LUMINANCE16;
				outFormat = GL_LUMINANCE16;
				outType = GL_UNSIGNED_SHORT;
				break;

			case PF_Alpha8:
				outInternalformat = GL_ALPHA8;
				outFormat = GL_ALPHA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_A4L4:
				outInternalformat = GL_LUMINANCE4_ALPHA4;
				outFormat = GL_LUMINANCE_ALPHA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_A8L8:
				outInternalformat = GL_LUMINANCE8_ALPHA8;
				outFormat = GL_LUMINANCE_ALPHA;
				outType = GL_UNSIGNED_SHORT;
				break;

			case PF_R5G6B5:
				outInternalformat = GL_RGB5;
				outFormat = GL_RGB;
				outType = GL_UNSIGNED_SHORT_5_6_5;
				break;

			case PF_B5G6R5:
				outInternalformat = GL_RGB5;
				outFormat = GL_BGR;
				outType = GL_UNSIGNED_SHORT_5_6_5;
				break;

			case PF_A4R4G4B4:
				outInternalformat = GL_RGBA4;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_SHORT_4_4_4_4_REV;
				break;

			case PF_A1R5G5B5:
				outInternalformat = GL_RGB5_A1;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
				break;

			case PF_R8G8B8:
				outInternalformat = GL_RGB8;
				outFormat = GL_BGR;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_B8G8R8:
				outInternalformat = GL_RGB8;
				outFormat = GL_RGB;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_A8R8G8B8:
				outInternalformat = GL_RGBA8;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_A8B8G8R8:
				outInternalformat = GL_RGBA8;
				outFormat = GL_RGBA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_A2R10G10B10:
				outInternalformat = GL_RGB10_A2;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_INT_2_10_10_10_REV;
				break;

			case PF_A2B10G10R10:
				outInternalformat = GL_RGB10_A2;
				outFormat = GL_RGBA;
				outType = GL_UNSIGNED_INT_2_10_10_10_REV;
				break;

			case PF_B16G16R16F:
				outInternalformat = GL_RGB16F;
				outFormat = GL_RGB;
				outType = GL_HALF_FLOAT;
				break;

			case PF_A16B16G16R16F:
				outInternalformat = GL_RGBA16F;
				outFormat = GL_RGBA;
				outType = GL_HALF_FLOAT;
				break;

			case PF_B32G32R32F:
				outInternalformat = GL_RGB32F;
				outFormat = GL_RGB;
				outType = GL_FLOAT;
				break;

			case PF_A32B32G32R32F:
				outInternalformat = GL_RGBA32F;
				outFormat = GL_RGBA;
				outType = GL_FLOAT;
				break;

			case PF_X8R8G8B8:
				outInternalformat = GL_RGB8;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_X8B8G8R8:
				outInternalformat = GL_RGB8;
				outFormat = GL_RGBA;
				outType = GL_UNSIGNED_BYTE;
				break;

				//////////////////////////////////////////////////////////////////////////
			case PF_R8G8B8A8:
				outInternalformat = GL_RGBA8;
				outFormat = GL_RGBA;
				outType = GL_UNSIGNED_BYTE;
				break;

			case PF_B8G8R8A8:
				outInternalformat = GL_RGBA8;
				outFormat = GL_BGRA;
				outType = GL_UNSIGNED_BYTE;
				break;

				//////////////////////////////////////////////////////////////////////////

			case PF_R3G3B2:
				outInternalformat = GL_R3_G3_B2;
				outFormat = GL_RGB;
				outType = GL_UNSIGNED_BYTE_3_3_2;
				break;

			case PF_R16F:
				outInternalformat = GL_LUMINANCE16F_ARB;
				outFormat = GL_LUMINANCE;
				outType = GL_HALF_FLOAT;
				break;

			case PF_R32F:
				outInternalformat = GL_LUMINANCE32F_ARB;
				outFormat = GL_LUMINANCE;
				outType = GL_FLOAT;
				break;

			case PF_G16R16:
				outInternalformat = GL_LUMINANCE16_ALPHA16;
				outFormat = GL_LUMINANCE_ALPHA;
				outType = GL_UNSIGNED_SHORT;
				break;

			case PF_G16R16F:
				outInternalformat = GL_LUMINANCE_ALPHA16F_ARB;
				outFormat = GL_LUMINANCE_ALPHA;
				outType = GL_HALF_FLOAT_ARB;
				break;

			case PF_G32R32F:
				outInternalformat = GL_LUMINANCE_ALPHA32F_ARB;
				outFormat = GL_LUMINANCE_ALPHA;
				outType = GL_FLOAT;
				break;

			case PF_B16G16R16:
				outInternalformat = GL_RGB16;
				outFormat = GL_RGB_INTEGER;
				outType = GL_UNSIGNED_SHORT;
				break;

			default:
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Pixel Format!", 
					"OpenGLMapping::Mapping( GLint& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat)");
			}
		}

		PixelFormat OpenGLMapping::UnMapping( GLint internalformat, GLenum format, GLenum type )
		{
			switch(internalformat)
			{
			case GL_LUMINANCE8:
				if(type == GL_UNSIGNED_BYTE)
					return PF_Luminance8;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE16:
				if(type == GL_UNSIGNED_SHORT)
					return PF_Luminance16;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_ALPHA8:
				if(type == GL_UNSIGNED_BYTE)
					return PF_Alpha8;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE4_ALPHA4:
				assert(format == GL_LUMINANCE_ALPHA);
				if(type == GL_UNSIGNED_BYTE)
					return PF_A4L4;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE8_ALPHA8:
				assert(format == GL_LUMINANCE_ALPHA);
				if(type == GL_UNSIGNED_SHORT)
					return PF_A8L8;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB5:
				assert(type == GL_UNSIGNED_SHORT_5_6_5);
				if(format == GL_RGB)
					return PF_R5G6B5;
				else if(format == GL_BGR)
					return PF_B5G6R5;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGBA4:
				// assert(type == GL_UNSIGNED_SHORT_4_4_4_4_REV);
				if(format == GL_BGRA)
					return PF_A4R4G4B4;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB5_A1:
				// assert(type == GL_UNSIGNED_SHORT_1_5_5_5_REV);

				if(format == GL_BGRA)
					return PF_A1R5G5B5;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB8: 
				assert(type == GL_UNSIGNED_BYTE);

				switch(format)
				{
				case GL_BGR:
					return PF_R8G8B8;
				case GL_RGB:
					return PF_B8G8R8;
				case GL_BGRA:
					return PF_X8R8G8B8;
				case GL_RGBA:
					return PF_X8B8G8R8;
				default:
					// not supported
					assert(false);
					return PF_Unknown;	
				}

			case GL_RGBA8:
				assert(type == GL_UNSIGNED_BYTE);
				switch(format)
				{
				case GL_BGR:
					return PF_X8R8G8B8;
				case GL_RGB:
					return PF_X8B8G8R8;
				case GL_BGRA:
					return PF_A8R8G8B8;
				case GL_RGBA:
					return PF_A8B8G8R8;
				default:
					// not supported
					assert(false);
					return PF_Unknown;	
				}

				//if(format == GL_BGRA)
				//	return PF_A8R8G8B8;
				//else if(format == GL_RGBA)
				//	return PF_A8B8G8R8;
				//else
				//{
				//	// not supported
				//	assert(false);
				//	return PF_Unknown;
				//}

			case PF_A2R10G10B10:
				//assert(type == GL_UNSIGNED_INT_2_10_10_10_REV);
				if(format == GL_BGRA)
					return PF_A2R10G10B10;
				else if(format == GL_RGBA)
					return PF_A2B10G10R10;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB16F:
				assert(type == GL_HALF_FLOAT);
				if(format == GL_RGB)
					return PF_B16G16R16F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}


			case GL_RGBA16F:
				assert(type == GL_HALF_FLOAT);
				if(format == GL_RGBA)
					return PF_A16B16G16R16F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB32F:
				assert(type == GL_FLOAT);
				if(format == GL_RGB)
					return PF_B32G32R32F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGBA32F:
				assert(type == GL_FLOAT);
				if(format == GL_RGBA)
					return PF_A32B32G32R32F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

				//////////////////////////////////////////////////////////////////////////

				//case PF_R8G8B8A8:
				//	outInternalformat = GL_RGBA8;
				//	outFormat = GL_RGBA;
				//	outType = GL_UNSIGNED_BYTE;
				//	break;

				//case PF_B8G8R8A8:
				//	outInternalformat = GL_RGBA8;
				//	outFormat = GL_BGRA;
				//	outType = GL_UNSIGNED_BYTE;
				//	break;

				//////////////////////////////////////////////////////////////////////////

			case GL_R3_G3_B2:
				assert(type == GL_UNSIGNED_BYTE_3_3_2);
				if(format == GL_RGB)
					return PF_R3G3B2;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE16F_ARB:
				assert(type == GL_HALF_FLOAT);
				if(format == GL_LUMINANCE)
					return PF_R16F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE32F_ARB:
				assert(type == GL_FLOAT);
				if(format == GL_LUMINANCE)
					return PF_R32F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE16_ALPHA16:
				assert(type == GL_UNSIGNED_SHORT);
				if(format == GL_LUMINANCE_ALPHA)
					return PF_G16R16;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE_ALPHA16F_ARB:
				assert(type == GL_HALF_FLOAT);
				if(format == GL_LUMINANCE_ALPHA)
					return PF_G16R16F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_LUMINANCE_ALPHA32F_ARB:
				assert(type == GL_FLOAT);
				if(format == GL_LUMINANCE_ALPHA)
					return PF_G32R32F;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			case GL_RGB16:
				assert(type == GL_UNSIGNED_SHORT);
				if(format == GL_RGB)
					return PF_B16G16R16;
				else
				{
					// not supported
					assert(false);
					return PF_Unknown;
				}

			default:
				// not supported
				assert(false);
				return PF_Unknown;
			}
		}


	}
}
