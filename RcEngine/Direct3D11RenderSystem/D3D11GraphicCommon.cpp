#include "D3D11GraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

void D3D11Mapping::MapUsage( uint32_t accessHint, D3D11_USAGE& usage, UINT& CPUAccessFlags )
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
	case RcEngine::PF_Count:
		break;
	default:
		break;
	}
}

D3D11_COMPARISON_FUNC D3D11Mapping::Mapping( CompareFunction cmpFunc )
{
	switch (cmpFunc)
	{
	case CF_Always:
		return 
	case CF_Never:
		break;
	case CF_Less:
		break;
	case CF_LessEqual:
		break;
	case RcEngine::CF_Equal:
		break;
	case RcEngine::CF_NotEqual:
		break;
	case RcEngine::CF_GreaterEqual:
		break;
	case RcEngine::CF_Greater:
		break;
	default:
		break;
	}
}

}