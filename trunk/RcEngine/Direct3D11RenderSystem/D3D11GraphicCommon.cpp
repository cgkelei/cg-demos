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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid access hint", "D3D11Mapping::MapUsage");
	}
}

DXGI_FORMAT D3D11Mapping::Mapping( PixelFormat inPixelFormat )
{
	switch (inPixelFormat)
	{
	case PF_Luminance8:
		return DXGI_FORMAT_R8_UNORM;
	case PF_Luminance16:
		return DXGI_FORMAT_R16_UNORM;
	case PF_Alpha8:
		return DXGI_FORMAT_A8_UNORM;
	case PF_A4L4:
		return DXGI_FORMAT_UNKNOWN;
	case PF_A8L8:
		return DXGI_FORMAT_R8G8_UNORM;
		break;

	case PF_R3G3B2:
		break;
	case PF_R5G6B5:
		break;
	case PF_B5G6R5:
		break;
	case PF_A4R4G4B4:
		return DXGI_FORMAT_B4G4R4A4_UNORM;
	case PF_A1R5G5B5:              
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case PF_R8G8B8:
		break;
	case RcEngine::PF_B8G8R8:
		return DXGI_FORMAT_UNKNOWN;
	case PF_A8R8G8B8:
		return DXGI_FORMAT_UNKNOWN;
	case PF_A8B8G8R8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PF_B8G8R8A8:
		return DXGI_FORMAT_UNKNOWN;
	case PF_A2R10G10B10:
		return DXGI_FORMAT_UNKNOWN;
	case PF_A2B10G10R10:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	
	case RcEngine::PF_B16G16R16F:
		break;
	case PF_A16B16G16R16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RcEngine::PF_B32G32R32F:
		break;
	case PF_A32B32G32R32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case PF_X8R8G8B8:
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	case PF_X8B8G8R8:
		return DXGI_FORMAT_UNKNOWN;
	
	case PF_R8G8B8A8:
		break;

	case PF_A16B16G16R16:
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	
	case PF_R16F:
		return DXGI_FORMAT_R16_FLOAT;
	case PF_R32F:
		return DXGI_FORMAT_R32_FLOAT;
	case PF_G16R16:
		return DXGI_FORMAT_R16G16_TYPELESS;
	case PF_G16R16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case PF_G32R32F:
		return DXGI_FORMAT_R32G32B32_FLOAT;


	case PF_DXT1:
		return DXGI_FORMAT_BC1_UNORM;
	case PF_DXT2:
		return DXGI_FORMAT_BC2_UNORM;
	case PF_DXT3:
		return  DXGI_FORMAT_BC3_UNORM;
	case PF_DXT4:
		return DXGI_FORMAT_BC4_UNORM;
	case RcEngine::PF_DXT5:
		return DXGI_FORMAT_BC5_UNORM;
	
	case RcEngine::PF_B16G16R16:
		break;
	case RcEngine::PF_Depth16:
		return DXGI_FORMAT_D16_UNORM;
	case PF_Depth24Stencil8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case RcEngine::PF_Depth32:
		return DXGI_FORMAT_D32_FLOAT;
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid PixelFormat", "D3D11Mapping::Mapping");
	}
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid CompareFunction", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid StencilOperation", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid BlendOperation", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid Blend", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid CullMode", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid FillMode", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid TextureAddressMode", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid TextureFilter", "D3D11Mapping::Mapping");
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
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid VertexElementFormat", "D3D11Mapping::Mapping");
	}
}


}