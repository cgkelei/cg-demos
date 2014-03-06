#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {


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
	case VEF_Float:
	case VEF_Float2:
	case VEF_Float3:
	case VEF_Float4:
		return GL_FLOAT;


	case VEF_Int:
	case VEF_Int2:
	case VEF_Int3:
	case VEF_Int4:
		return GL_INT;


	case VEF_UInt:
	case VEF_UInt2:
	case VEF_UInt3:
	case VEF_UInt4:
		return GL_UNSIGNED_INT;


	case VEF_Bool:
	case VEF_Bool2:
	case VEF_Bool3:
	case VEF_Bool4:
		return GL_BOOL;

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

void OpenGLMapping::Mapping( GLenum& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat )
{
	switch(inPixelFormat)
	{
	case PF_Luminance8:
		outInternalformat = GL_LUMINANCE8;
		outFormat = GL_LUMINANCE;
		outType = GL_UNSIGNED_BYTE;
		break;

	case PF_Luminance16:
		outInternalformat = GL_LUMINANCE16;
		outFormat = GL_LUMINANCE;
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

	case PF_A8B8G8R8: //
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
		outInternalformat = GL_R16F;
		outFormat = GL_RED;
		outType = GL_HALF_FLOAT;
		break;

	case PF_R32F:
		outInternalformat = GL_R32F;
		outFormat = GL_RED;
		outType = GL_FLOAT;
		break;

	case PF_G16R16:
		outInternalformat = GL_RG16;
		outFormat = GL_RG;
		outType = GL_UNSIGNED_SHORT;
		break;

	case PF_G16R16F:
		outInternalformat = GL_RG16F;
		outFormat = GL_RG;
		outType = GL_HALF_FLOAT;
		break;

	case PF_G32R32F://
		outInternalformat = GL_RG32F;
		outFormat = GL_RG;
		outType = GL_FLOAT;
		break;

	case PF_B16G16R16:
		outInternalformat = GL_RGB16;
		outFormat = GL_RGB;
		outType = GL_UNSIGNED_SHORT;
		break;

	case PF_DXT1:
		outInternalformat =  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		outFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		outType = GL_UNSIGNED_BYTE;
		break;

	case PF_DXT3:
		outInternalformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		outFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		outType = GL_UNSIGNED_BYTE;
		break;

	case PF_DXT5:
		outInternalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		outFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		outType = GL_UNSIGNED_BYTE;
		break;

	case PF_Depth16:
		outInternalformat = GL_DEPTH_COMPONENT16;
		outFormat = GL_DEPTH_COMPONENT;
		outType = GL_UNSIGNED_SHORT;
		break;

	case PF_Depth24Stencil8:
		if (GLEW_EXT_packed_depth_stencil)
		{
			outInternalformat = GL_DEPTH24_STENCIL8_EXT;
			outFormat = GL_DEPTH_STENCIL_EXT;
			outType = GL_UNSIGNED_INT_24_8_EXT;
		}
		else
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported render texture:" + PixelFormatUtils::GetFormatName(PF_Depth24Stencil8), "OpenGLMapping::Mapping");
		}
		break;

	case PF_Depth32://
		outInternalformat = GL_DEPTH_COMPONENT32F;
		outFormat = GL_DEPTH_COMPONENT;
		outType = GL_FLOAT;
		break;	
	
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Pixel Format!", 
			"OpenGLMapping::Mapping( GLint& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat)");
	}
}

GLenum OpenGLMapping::Mapping( BlendOperation op )
{
	switch(op)
	{
	case BOP_Add:
		return GL_FUNC_ADD;
	case BOP_Sub:
		return GL_FUNC_SUBTRACT;
	case BOP_Rev_Sub:
		return GL_FUNC_REVERSE_SUBTRACT;
	case BOP_Min:
		return GL_MIN;
	case BOP_Max:
		return GL_MAX;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported BlendOperation!", 
			"OpenGLMapping::Mapping( BlendOperation op )");
	}
}

GLenum OpenGLMapping::Mapping( AlphaBlendFactor factor )
{
	switch(factor)
	{
	case ABF_Zero:
		return GL_ZERO;

	case ABF_One:
		return GL_ONE;

	case ABF_Src_Alpha:
		return GL_SRC_ALPHA;

	case ABF_Dst_Alpha:
		return GL_DST_ALPHA;

	case ABF_Inv_Src_Alpha:
		return GL_ONE_MINUS_SRC_ALPHA;

	case ABF_Inv_Dst_Alpha:
		return GL_ONE_MINUS_DST_ALPHA;

	case ABF_Src_Color:
		return GL_SRC_COLOR;

	case ABF_Dst_Color:
		return GL_DST_COLOR;

	case ABF_Inv_Src_Color:
		return GL_ONE_MINUS_SRC_COLOR;

	case ABF_Inv_Dst_Color:
		return GL_ONE_MINUS_DST_COLOR;

	case ABF_Src_Alpha_Sat:
		return GL_SRC_ALPHA_SATURATE;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported AlphaBlendFactor!", 
			"OpenGLMapping::Mapping( AlphaBlendFactor factor )");
	}
}

GLenum OpenGLMapping::Mapping( FillMode fm )
{
	switch(fm)
	{
	case FM_Solid:
		return GL_FILL;
	case FM_WireFrame:
		return GL_LINE;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported FillMode!", 
			"OpenGLMapping::Mapping( FillMode fm )");
	}
}

GLenum OpenGLMapping::Mapping( CompareFunction cf )
{
	switch(cf)
	{
	case CF_Always:
		return GL_ALWAYS;
	case CF_Never:
		return GL_NEVER;
	case CF_Less:
		return GL_LESS;
	case CF_LessEqual:
		return GL_LEQUAL;
	case CF_Equal:
		return GL_EQUAL;
	case CF_NotEqual:
		return GL_NOTEQUAL;
	case CF_GreaterEqual:
		return GL_GEQUAL;
	case CF_Greater:
		return GL_GREATER;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported CompareFunction!", 
			"OpenGLMapping::Mapping( CompareFunction cf )");
	}
}

GLenum OpenGLMapping::Mapping( StencilOperation sop )
{				
	switch(sop)
	{
	case SOP_Keep:
		return GL_KEEP;
	case SOP_Zero:
		return GL_ZERO;
	case SOP_Replace:
		return GL_REPLACE;
	case SOP_Incr:
		return GL_INCR;
	case SOP_Decr:
		return GL_DECR;
	case SOP_Invert:
		return GL_INVERT;
	case SOP_Incr_Wrap:
		return GL_INCR_WRAP;
	case SOP_Decr_Wrap:
		return GL_DECR_WRAP;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported StencilOperation!", 
			"OpenGLMapping::Mapping( StencilOperation )");
	}
}

GLenum OpenGLMapping::Mapping( TextureAddressMode mode )
{
	switch (mode)
	{
	case TAM_Wrap:
		return GL_REPEAT;
	case TAM_Border:
		return GL_CLAMP_TO_BORDER;
	case TAM_Clamp:
		return GL_CLAMP_TO_EDGE;
	case TAM_Mirror:
		return GL_MIRRORED_REPEAT;

	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported TextureAddressMode!", 
			"OpenGLMapping::Mapping( TextureAddressMode )");
	}
}

void OpenGLMapping::Mapping( GLenum& min, GLenum& mag, TextureFilter filter )
{
	switch(filter)
	{
	case TF_Min_Mag_Mip_Point:
		{
			min = GL_NEAREST;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Mag_Point_Mip_Linear:
		{
			min = GL_NEAREST_MIPMAP_LINEAR;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Point_Mag_Linear_Mip_Point:
		{
			min = GL_NEAREST_MIPMAP_NEAREST;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Point_Mag_Mip_Linear:
		{
			min = GL_NEAREST_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Linear_Mag_Mip_Point:
		{
			min = GL_LINEAR_MIPMAP_NEAREST;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Linear_Mag_Point_Mip_Linear:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_NEAREST;
		}
		break;
	case TF_Min_Mag_Linear_Mip_Point:
		{
			min = GL_LINEAR_MIPMAP_NEAREST;
			mag = GL_LINEAR;
		}
		break;
	case TF_Min_Mag_Mip_Linear:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	case TF_Anisotropic:
		{
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		}
		break;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Texture Filter", 
			"OpenGLMapping::Mapping");
	}
}

PixelFormat OpenGLMapping::UnMapping( GLenum internalformat, GLenum format, GLenum type )
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

	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
		return PF_DXT1;
				
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
		return PF_DXT3;

	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
		return PF_DXT5;

	default:
		// not supported
		assert(false);
		return PF_Unknown;
	}
}

void OpenGLMapping::UnMapping( EffectParameterType& outType, GLenum glType )
{
	switch (glType)
	{
	case GL_FLOAT:
		{
			outType = EPT_Float;
			break;
		}
	case GL_FLOAT_VEC2:
		{
			outType = EPT_Float2;
			break;
		}
	case GL_FLOAT_VEC3:
		{
			outType = EPT_Float3;
			break;
		}
	case GL_FLOAT_VEC4:
		{
			outType = EPT_Float4;
			break;
		}
	case GL_UNSIGNED_INT:
		{
			outType = EPT_Uint;
			break;
		}
	case GL_UNSIGNED_INT_VEC2:
		{
			outType = EPT_Uint2;
			break;
		}
	case GL_UNSIGNED_INT_VEC3:
		{
			outType = EPT_Uint3;
			break;
		}
	case GL_UNSIGNED_INT_VEC4:
		{
			outType = EPT_Uint4;
			break;
		}
	case GL_INT:
		{
			outType = EPT_Int;
			break;
		}
	case GL_INT_VEC2:
		{
			outType = EPT_Int2;
			break;
		}
	case GL_INT_VEC3:
		{
			outType = EPT_Int3;
			break;
		}
	case GL_INT_VEC4:
		{
			outType = EPT_Int4;
			break;
		}
	case GL_BOOL:
		{
			outType = EPT_Boolean;
			break;
		}
	case GL_FLOAT_MAT2:
		{
			outType = EPT_Matrix2x2;
			break;
		}
	case GL_FLOAT_MAT3:
		{
			outType = EPT_Matrix3x3;
			break;
		}
	case GL_FLOAT_MAT4:
		{
			outType = EPT_Matrix4x4;
			break;
		}
	case GL_SAMPLER_1D:
	case GL_SAMPLER_1D_SHADOW:
		{
			outType = EPT_Texture1D;
			break;
		}
	case GL_SAMPLER_2D:
	case GL_SAMPLER_2D_SHADOW:
		{
			outType = EPT_Texture2D;
			break;
		}
	case GL_SAMPLER_3D:
		{
			outType = EPT_Texture3D;
			break;
		}
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_CUBE_SHADOW:
		{
			outType = EPT_TextureCUBE;
			break;
		}
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
		{
			outType = EPT_Texture1DArray;
			break;
		}
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
		{
			outType = EPT_Texture2DArray;
			break;
		}
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unsupported Shader Parameter Type", 
			"OpenGLMapping::UnMapping");
	}
}

bool OpenGLMapping::IsIntegerType( GLenum type )
{
	switch(type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_INT:
	case GL_UNSIGNED_INT:
		return true;

	default:
		return false;
	}

	return false;
}

}
