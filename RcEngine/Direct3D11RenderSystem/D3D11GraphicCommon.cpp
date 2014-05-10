#include "D3D11GraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

void D3D11Mapping::Mapping( uint32_t accessHint, D3D11_USAGE& usage, UINT& CPUAccessFlags )
{
	CPUAccessFlags = 0;
	if (accessHint & EAH_CPU_Read)
		CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	if (accessHint & EAH_CPU_Write)
		CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			
	usage = D3D11_USAGE_DEFAULT;

	if (accessHint & EAH_GPU_Write)
	{
		usage = D3D11_USAGE_DEFAULT;
	}
	else if (accessHint & EAH_CPU_Write)
	{
		usage = D3D11_USAGE_DYNAMIC;
	}
	else if (accessHint == EAH_GPU_Read)
	{
		// Only sample by GPU
		usage = D3D11_USAGE_IMMUTABLE;
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid access hint", "D3D11Mapping::MapUsage");
	}
}

DXGI_FORMAT D3D11Mapping::Mapping( PixelFormat inPixelFormat )
{
	static const DXGI_FORMAT d3d11Format[PF_Count] = {

		DXGI_FORMAT_UNKNOWN, //PF_Unknown = 0,

		// unsigned normalized formats
		DXGI_FORMAT_R8_UNORM,//PF_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,//PF_RG8_UNORM,
		DXGI_FORMAT_UNKNOWN, //PF_RGB8_UNORM,
		DXGI_FORMAT_UNKNOWN, //PF_BGR8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,//PF_RGBA8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM,//PF_BGRA8_UNORM,
		DXGI_FORMAT_UNKNOWN, //PF_RGBX8_UNORM,
		DXGI_FORMAT_B8G8R8X8_UNORM,//PF_BGRX8_UNORM,

		DXGI_FORMAT_R16_UNORM,//PF_R16_UNORM,
		DXGI_FORMAT_R16G16_UNORM,//PF_RG16_UNORM,
		DXGI_FORMAT_UNKNOWN,//PF_RGB16_UNORM,
		DXGI_FORMAT_R16G16B16A16_UNORM,//PF_RGBA16_UNORM,

		// snorm formats
		DXGI_FORMAT_R8_SNORM,//PF_R8_SNORM,
		DXGI_FORMAT_R8G8_SNORM,//PF_RG8_SNORM,
		DXGI_FORMAT_UNKNOWN,//PF_RGB8_SNORM,
		DXGI_FORMAT_R8G8B8A8_SNORM,//PF_RGBA8_SNORM,

		DXGI_FORMAT_R16_SNORM,//PF_R16_SNORM,
		DXGI_FORMAT_R16G16_SNORM,//PF_RG16_SNORM,
		DXGI_FORMAT_UNKNOWN,//PF_RGB16_SNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,//PF_RGBA16_SNORM,

		// Unsigned integer formats
		DXGI_FORMAT_R8_UINT,//PF_R8U,
		DXGI_FORMAT_R8G8_UINT,//PF_RG8U,
		DXGI_FORMAT_UNKNOWN,//PF_RGB8U,
		DXGI_FORMAT_R8G8B8A8_UINT,//PF_RGBA8U,

		DXGI_FORMAT_R16_UINT,//PF_R16U,
		DXGI_FORMAT_R16G16_UINT,//PF_RG16U,
		DXGI_FORMAT_UNKNOWN,//PF_RGB16U,
		DXGI_FORMAT_R16G16B16A16_UINT,//PF_RGBA16U,

		DXGI_FORMAT_R32_UINT,//PF_R32U,
		DXGI_FORMAT_R32G32_UINT,//PF_RG32U,
		DXGI_FORMAT_R32G32B32_UINT,//PF_RGB32U,
		DXGI_FORMAT_R32G32B32A32_UINT,//PF_RGBA32U,

		// Signed integer formats
		DXGI_FORMAT_R8_SINT,//PF_R8I,
		DXGI_FORMAT_R8G8_SINT,//PF_RG8I,
		DXGI_FORMAT_UNKNOWN,//PF_RGB8I,
		DXGI_FORMAT_R8G8B8A8_SINT,//PF_RGBA8I,
	
		DXGI_FORMAT_R16_SINT,//PF_R16I,
		DXGI_FORMAT_R16G16_SINT,//PF_RG16I,
		DXGI_FORMAT_UNKNOWN,//PF_RGB16I,
		DXGI_FORMAT_R16G16B16A16_SINT,//PF_RGBA16I,

		DXGI_FORMAT_R32_SINT,//PF_R32I,
		DXGI_FORMAT_R32G32_SINT,//PF_RG32I,
		DXGI_FORMAT_R32G32B32_SINT,//PF_RGB32I,
		DXGI_FORMAT_R32G32B32A32_SINT,//PF_RGBA32I,

		// Floating formats
		DXGI_FORMAT_R16_FLOAT,//PF_R16F,
		DXGI_FORMAT_R16G16_FLOAT,//PF_RG16F,
		DXGI_FORMAT_UNKNOWN,//PF_RGB16F,
		DXGI_FORMAT_R16G16B16A16_FLOAT,//PF_RGBA16F,

		DXGI_FORMAT_R32_FLOAT,//PF_R32F,
		DXGI_FORMAT_R32G32_FLOAT,//PF_RG32F,
		DXGI_FORMAT_R32G32B32_FLOAT,//PF_RGB32F,
		DXGI_FORMAT_R32G32B32A32_FLOAT,//PF_RGBA32F,
	
		// Packed formats
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP,//PF_RGB9E5,
		DXGI_FORMAT_R11G11B10_FLOAT,//PF_RG11B10F,
		DXGI_FORMAT_UNKNOWN,//PF_R3G3B2,
		DXGI_FORMAT_B5G6R5_UNORM,//PF_B5G6R5,
		DXGI_FORMAT_B5G5R5A1_UNORM,//PF_RGB5A1,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA4,
		DXGI_FORMAT_R10G10B10A2_UNORM,//PF_RGB10A2,
	
		// Depth formats
		DXGI_FORMAT_D16_UNORM,//PF_D16,
		DXGI_FORMAT_UNKNOWN,//PF_D24X8,
		DXGI_FORMAT_D24_UNORM_S8_UINT,//PF_D24S8,
		DXGI_FORMAT_D32_FLOAT,//PF_D32F,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,//PF_D32FS8X24,

		// Compressed formats
		DXGI_FORMAT_BC1_UNORM,//PF_RGB_DXT1,
		DXGI_FORMAT_BC1_UNORM,//PF_RGBA_DXT1,
		DXGI_FORMAT_BC2_UNORM,//PF_RGBA_DXT3,
		DXGI_FORMAT_BC3_UNORM,//PF_RGBA_DXT5,
		DXGI_FORMAT_BC4_UNORM,//PF_R_ATI1N_UNORM,
		DXGI_FORMAT_BC4_SNORM,//PF_R_ATI1N_SNORM,
		DXGI_FORMAT_BC5_UNORM,//PF_RG_ATI2N_UNORM,
		DXGI_FORMAT_BC5_SNORM,//PF_RG_ATI2N_SNORM,
		DXGI_FORMAT_BC6H_UF16,//PF_RGB_BP_UNSIGNED_FLOAT,
		DXGI_FORMAT_BC6H_SF16,//PF_RGB_BP_SIGNED_FLOAT,
		DXGI_FORMAT_BC7_UNORM,//PF_RGB_BP_UNORM,
		DXGI_FORMAT_UNKNOWN,//PF_RGB_PVRTC_4BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_RGB_PVRTC_2BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_PVRTC_4BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_PVRTC_2BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_ATC_RGB,
		DXGI_FORMAT_UNKNOWN,//PF_ATC_RGBA_EXPLICIT_ALPHA,
		DXGI_FORMAT_UNKNOWN,//PF_ATC_RGBA_INTERPOLATED_ALPHA,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_4x4,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_5x4,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_5x5,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_6x5,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_6x6,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_8x5,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_8x6,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_8x8,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_10x5,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_10x6,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_10x8,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_10x10,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_12x10,
		DXGI_FORMAT_UNKNOWN,//PF_RGBA_ASTC_12x12,

		// sRGB formats
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_UNORM,
		DXGI_FORMAT_UNKNOWN,//PF_SBGR8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,//PF_SRGB8_ALPHA8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,//PF_SBGR8_ALPHA8_UNORM,
		DXGI_FORMAT_UNKNOWN,//PF_SRGBX8_UNORM,
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,//PF_SBGRX8_UNORM,
		DXGI_FORMAT_BC1_UNORM_SRGB,//PF_SRGB_DXT1,
		DXGI_FORMAT_BC1_UNORM_SRGB,//PF_SRGB_ALPHA_DXT1,
		DXGI_FORMAT_BC2_UNORM_SRGB,//PF_SRGB_ALPHA_DXT3,
		DXGI_FORMAT_BC3_UNORM_SRGB,//PF_SRGB_ALPHA_DXT5,
		DXGI_FORMAT_BC7_UNORM_SRGB,//PF_SRGB_BP_UNORM,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB_PVRTC_2BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB_PVRTC_4BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB_ALPHA_PVRTC_2BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB_ALPHA_PVRTC_4BPPV1,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_4x4,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_5x4,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_5x5,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_6x5,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_6x6,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_8x5,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_8x6,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_8x8,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_10x5,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_10x6,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_10x8,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_10x10,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_12x10,
		DXGI_FORMAT_UNKNOWN,//PF_SRGB8_ALPHA8_ASTC_12x12,
	};

	int count = sizeof(d3d11Format) / sizeof(d3d11Format[0]);
	assert(count == PF_Count);

	if (d3d11Format[inPixelFormat] == DXGI_FORMAT_UNKNOWN)
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Unsupported format in D3D11", "D3D11Mapping::Mapping");

	return d3d11Format[inPixelFormat];
}

D3D11_COMPARISON_FUNC D3D11Mapping::Mapping( CompareFunction cmpFunc )
{
	switch (cmpFunc)
	{
	case CF_Always:
		return D3D11_COMPARISON_ALWAYS;
	case CF_Never:
		return D3D11_COMPARISON_NEVER;
	case CF_Less:
		return D3D11_COMPARISON_LESS;
	case CF_LessEqual:
		return D3D11_COMPARISON_LESS_EQUAL;
	case CF_Equal:
		return D3D11_COMPARISON_EQUAL;
	case CF_NotEqual:
		return D3D11_COMPARISON_NOT_EQUAL;
	case CF_GreaterEqual:
		return D3D11_COMPARISON_GREATER_EQUAL;
	case CF_Greater:
		return D3D11_COMPARISON_GREATER;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid CompareFunction", "D3D11Mapping::Mapping");
	}
}

D3D11_STENCIL_OP D3D11Mapping::Mapping( StencilOperation sop )
{
	switch(sop)
	{
	case SOP_Zero:
		return D3D11_STENCIL_OP_ZERO;
	case SOP_Keep:
		return D3D11_STENCIL_OP_KEEP;
	case SOP_Replace:
		return D3D11_STENCIL_OP_REPLACE;
	case SOP_Incr:
		return D3D11_STENCIL_OP_INCR;
	case SOP_Invert:
		return D3D11_STENCIL_OP_INVERT;
	case SOP_Decr:
		return D3D11_STENCIL_OP_DECR;
	case SOP_Incr_Wrap:
		return D3D11_STENCIL_OP_INCR_SAT;
	case SOP_Decr_Wrap:
		return D3D11_STENCIL_OP_DECR_SAT;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid StencilOperation", "D3D11Mapping::Mapping");
	}
}

D3D11_BLEND_OP D3D11Mapping::Mapping( BlendOperation bop )
{
	switch (bop)
	{
	case BOP_Add:
		return D3D11_BLEND_OP_ADD;
	case BOP_Sub:
		return D3D11_BLEND_OP_SUBTRACT;
	case BOP_Rev_Sub:
		return D3D11_BLEND_OP_REV_SUBTRACT;
	case BOP_Min:
		return D3D11_BLEND_OP_MIN;
	case BOP_Max:
		return D3D11_BLEND_OP_MAX;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid BlendOperation", "D3D11Mapping::Mapping");
	}
}

D3D11_BLEND D3D11Mapping::Mapping( AlphaBlendFactor blend )
{
	switch (blend)
	{
	case ABF_Zero:
		return D3D11_BLEND_ZERO;
	case ABF_One:
		return D3D11_BLEND_ONE;
	case ABF_Src_Alpha:
		return D3D11_BLEND_SRC_ALPHA;
	case ABF_Dst_Alpha:
		return D3D11_BLEND_DEST_ALPHA;
	case ABF_Inv_Src_Alpha:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case ABF_Inv_Dst_Alpha:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case ABF_Src_Color:
		return D3D11_BLEND_SRC_COLOR;
	case ABF_Dst_Color:
		return D3D11_BLEND_DEST_COLOR;
	case ABF_Inv_Src_Color:
		return D3D11_BLEND_INV_SRC_COLOR;
	case ABF_Inv_Dst_Color:
		return D3D11_BLEND_INV_DEST_COLOR;
	case ABF_Src_Alpha_Sat:
		return D3D11_BLEND_SRC_ALPHA_SAT;
	case ABF_Blend_Factor:
		return D3D11_BLEND_BLEND_FACTOR;
	case ABF_Inv_Blend_Factor:
		return D3D11_BLEND_INV_BLEND_FACTOR;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid Blend", "D3D11Mapping::Mapping");
	}
}

D3D11_CULL_MODE D3D11Mapping::Mapping( CullMode mode )
{
	switch (mode)
	{
	case CM_None:
		return D3D11_CULL_NONE;
	case CM_Front:
		return D3D11_CULL_FRONT;
	case CM_Back:
		return D3D11_CULL_BACK;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid CullMode", "D3D11Mapping::Mapping");
	}
}

D3D11_FILL_MODE D3D11Mapping::Mapping( FillMode mode )
{
	switch (mode)
	{
	case FM_Solid:
		return D3D11_FILL_SOLID;
	case FM_WireFrame:
		return D3D11_FILL_WIREFRAME;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid FillMode", "D3D11Mapping::Mapping");
	}
}

D3D11_TEXTURE_ADDRESS_MODE D3D11Mapping::Mapping( TextureAddressMode mode )
{
	switch (mode)
	{
	case TAM_Wrap:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case TAM_Mirror:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TAM_Clamp:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case TAM_Border:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	case TAM_Mirror_Once:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid TextureAddressMode", "D3D11Mapping::Mapping");
	}
}

D3D11_FILTER D3D11Mapping::Mapping( TextureFilter filter, bool compare /*= false*/ )
{
	switch (filter)
	{
	case TF_Min_Mag_Mip_Point:
		return compare ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
	case TF_Min_Mag_Point_Mip_Linear:
		return compare ? D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	case TF_Min_Point_Mag_Linear_Mip_Point:
		return compare ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case TF_Min_Point_Mag_Mip_Linear:
		return compare ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR : D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	case TF_Min_Linear_Mag_Mip_Point:
		return compare ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT : D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	case TF_Min_Linear_Mag_Point_Mip_Linear:
		return compare ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case TF_Min_Mag_Linear_Mip_Point:
		return compare ? D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	case TF_Min_Mag_Mip_Linear:
		return compare ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case TF_Anisotropic:
		return compare ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid TextureFilter", "D3D11Mapping::Mapping");
	}
}

uint32_t D3D11Mapping::Mapping( uint32_t bufferCreateFlags )
{
	uint32_t bindFlag = 0;

	if (bufferCreateFlags & BufferCreate_Uniform)
	{
		bindFlag = D3D11_BIND_CONSTANT_BUFFER;
	}
	else if (bufferCreateFlags & BufferCreate_Index)
	{
		bindFlag = D3D11_BIND_INDEX_BUFFER;
	}
	else
	{
		if (bufferCreateFlags & BufferCreate_Vertex)
			bindFlag |= D3D11_BIND_VERTEX_BUFFER;
		
		if (bufferCreateFlags & BufferCreate_StreamOutput)
			bindFlag |= D3D11_BIND_STREAM_OUTPUT;

		if (bufferCreateFlags & (BufferCreate_Texture | BufferCreate_Structured) )
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
	}

	return bindFlag;
}

DXGI_FORMAT D3D11Mapping::Mapping( VertexElementFormat format )
{
	switch (format)
	{
	case VEF_Float: return DXGI_FORMAT_R32_FLOAT;
	case VEF_Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case VEF_Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case VEF_Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case VEF_Int: return DXGI_FORMAT_R32_SINT;
	case VEF_Int2: return DXGI_FORMAT_R32G32_SINT;
	case VEF_Int3: return DXGI_FORMAT_R32G32B32_SINT;
	case VEF_Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
	case VEF_UInt: return DXGI_FORMAT_R32_UINT;
	case VEF_UInt2: return DXGI_FORMAT_R32G32_UINT;
	case VEF_UInt3: return DXGI_FORMAT_R32G32B32_UINT;
	case VEF_UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Invalid VertexElementFormat", "D3D11Mapping::Mapping");
	}
}

D3D10_PRIMITIVE_TOPOLOGY D3D11Mapping::Mapping( PrimitiveType primType )
{
	switch (primType)
	{
	case PT_Point_List:				return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PT_Line_List:				return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PT_Line_List_Adj:			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case PT_Line_Strip:				return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PT_Line_Strip_Adj:			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case PT_Triangle_List:			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PT_Triangle_List_Adj:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case PT_Triangle_Strip:			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case PT_Triangle_Strip_Adj:		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	default:
		return D3D10_PRIMITIVE_TOPOLOGY(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + primType - PT_Patch_Control_Point_1);
	}
}

D3D11_MAP D3D11Mapping::Mapping( ResourceMapAccess map )
{
	switch (map)
	{
	case RMA_Read_Only:				return D3D11_MAP_READ;
	case RMA_Write_Only:			return D3D11_MAP_WRITE ;
	case RMA_Read_Write:			return D3D11_MAP_READ_WRITE ;
	case RMA_Write_Discard:			return D3D11_MAP_WRITE_DISCARD;
	case RMA_Write_No_Overwrite:	return D3D11_MAP_WRITE_NO_OVERWRITE;
	}

	assert(false);
	return D3D11_MAP_WRITE_DISCARD;
}

void D3D11Mapping::UnMapping( D3D10_SRV_DIMENSION dimension, EffectParameterType& oTexType )
{
	switch (dimension)
	{
	case D3D10_SRV_DIMENSION_TEXTURE1D:
		{
			oTexType = EPT_Texture1D;
		}
		break;
	case D3D10_SRV_DIMENSION_TEXTURE1DARRAY:
		{
			oTexType = EPT_Texture1DArray;
		}
		break;
	case D3D10_SRV_DIMENSION_TEXTURE2D:
	case D3D10_SRV_DIMENSION_TEXTURE2DMS:
		{
			oTexType = EPT_Texture2D;
		}
		break;
	case D3D10_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY:
		{
			oTexType = EPT_Texture2DArray;
		}
		break;
	case D3D10_SRV_DIMENSION_TEXTURE3D:
		{
			oTexType = EPT_Texture3D;
		}
		break;
	case D3D10_SRV_DIMENSION_TEXTURECUBE:
		{
			oTexType = EPT_TextureCube;
		}
		break;
	default:
		assert(false);
		break;
	}
}


}