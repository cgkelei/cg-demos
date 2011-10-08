#include "D3D9Mapping.h"
#include "Exception.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		D3DMULTISAMPLE_TYPE D3D9Mapping::Mapping( uint32 SampleCount )
		{
			assert( 0 <= SampleCount && SampleCount <= 16 );
			return static_cast<D3DMULTISAMPLE_TYPE>(SampleCount);
		}

		D3DFORMAT D3D9Mapping::Mapping( PixelFormat fmt )
		{
			switch(fmt)
			{
			case PF_Luminance8:
				return D3DFMT_L8;
			case PF_Luminance16:
				return D3DFMT_L16;
			case PF_Alpha8:
				return D3DFMT_A8;
			case PF_A4L4:
				return D3DFMT_A4L4;
			case PF_A8L8:
				return D3DFMT_A8L8;
			case PF_R3G3B2:
				return D3DFMT_R3G3B2;
			case PF_A1R5G5B5:
				return D3DFMT_A1R5G5B5;
			case PF_R5G6B5:
				return D3DFMT_R5G6B5;
			case PF_A4R4G4B4:
				return D3DFMT_A4R4G4B4;
			case PF_R8G8B8:
				return D3DFMT_R8G8B8;
			case PF_A8R8G8B8:
				return D3DFMT_A8R8G8B8;
			case PF_A8B8G8R8:
				return D3DFMT_A8B8G8R8;
			case PF_X8R8G8B8:
				return D3DFMT_X8R8G8B8;
			case PF_X8B8G8R8:
				return D3DFMT_X8B8G8R8;
			case PF_A2B10G10R10:
				return D3DFMT_A2B10G10R10;
			case PF_A2R10G10B10:
				return D3DFMT_A2R10G10B10;
			case PF_Float16_R:
				return D3DFMT_R16F;
			case PF_Float16_GR:
				return D3DFMT_G16R16F;
			case PF_Float16_RGBA:
				return D3DFMT_A16B16G16R16F;
			case PF_Float32_R:
				return D3DFMT_R32F;
			case PF_Float32_GR:
				return D3DFMT_G32R32F;
			case PF_Float32_RGBA:
				return D3DFMT_A32B32G32R32F;
			case PF_Short_RGBA:
				return D3DFMT_A16B16G16R16;
			case PF_Short_GR:
				return D3DFMT_G16R16;
			case PF_DXT1:
				return D3DFMT_DXT1;
			case PF_DXT2:
				return D3DFMT_DXT2;
			case PF_DXT3:
				return D3DFMT_DXT3;
			case PF_DXT4:
				return D3DFMT_DXT4;
			case PF_DXT5:
				return D3DFMT_DXT5;
			case PF_Unknown:
			default:
				return D3DFMT_UNKNOWN;
			}
		}

		D3DSHADEMODE D3D9Mapping::Mapping( ShadingType st )
		{
			switch( st )
			{
			case ST_Flat:
				return D3DSHADE_FLAT;
			case ST_Gouraud:
				return D3DSHADE_GOURAUD;
			case ST_Phong:
				return D3DSHADE_PHONG;
			}

			assert(false);
			return D3DSHADE_FORCE_DWORD   ;
		}

		D3DCMPFUNC  D3D9Mapping::Mapping( CompareFunction cf )
		{
			switch( cf )
			{
			case CMPF_Always_Fail:
				return D3DCMP_NEVER;

			case CMPF_Always_Pass:
				return D3DCMP_ALWAYS;

			case CMPF_Less:
				return D3DCMP_LESS;

			case CMPF_Less_Equal:
				return D3DCMP_LESSEQUAL;

			case CMPF_Equal:
				return D3DCMP_EQUAL;

			case CMPF_Not_Equal:
				return D3DCMP_NOTEQUAL;

			case CMPF_Greater_Equal:
				return D3DCMP_GREATEREQUAL;

			case CMPF_Greater:
				return D3DCMP_GREATER;
			};
			return D3DCMP_FORCE_DWORD;
		}
		

		Engine::CompareFunction D3D9Mapping::UnMapping( D3DCMPFUNC cf )
		{
			switch( cf )
			{
			case D3DCMP_NEVER :
				return CMPF_Always_Fail;

			case  D3DCMP_ALWAYS :
				return CMPF_Always_Pass;

			case D3DCMP_LESS :
				return CMPF_Less;

			case D3DCMP_LESSEQUAL :
				return CMPF_Less_Equal;

			case D3DCMP_EQUAL :
				return CMPF_Equal;

			case D3DCMP_NOTEQUAL :
				return CMPF_Not_Equal;

			case D3DCMP_GREATEREQUAL :
				return CMPF_Greater_Equal;

			case D3DCMP_GREATER:
				return  CMPF_Greater;
			};

			assert(false);
			return CMPF_Always_Fail;
		}

		D3DCULL D3D9Mapping::Mapping( CullingMode cm )
		{
			switch( cm )
			{
			case CULL_None:
				return D3DCULL_NONE;

			case CULL_Clockwise:
					return D3DCULL_CW;

			case CULL_CounterClockwise:
					return D3DCULL_CCW;
			}
			// to keep compiler happy
			assert(false);
			return D3DCULL_FORCE_DWORD;
		}

		CullingMode D3D9Mapping::UnMapping( D3DCULL  cm )
		{
			switch(cm)
			{
			case D3DCULL_NONE:
				return CULL_None;
			case D3DCULL_CW:
				return CULL_Clockwise;
			case D3DCULL_CCW:
				return CULL_CounterClockwise;
			}
			// to keep compiler happy
			assert(false);
			return CULL_CounterClockwise;
		}


		D3DFILLMODE D3D9Mapping::Mapping( PolygonMode fm )
		{
			switch(fm)
			{
			case FM_Points:
				return D3DFILL_POINT;

			case FM_WireFrame:
				return D3DFILL_WIREFRAME;

			case FM_Solid:
				return D3DFILL_SOLID;
			}
			// to keep compiler happy
			assert(false);
			return D3DFILL_FORCE_DWORD;
		}


		PolygonMode D3D9Mapping::UnMapping( D3DFILLMODE  fm )
		{
			switch(fm)
			{
			case D3DFILL_POINT:
				return FM_Points;

			case D3DFILL_WIREFRAME :
				return FM_WireFrame;

			case D3DFILL_SOLID :
				return FM_Solid;
			}
			// to keep compiler happy
			assert(false);
			return FM_Solid;
		}

		StencilOperation D3D9Mapping::UnMapping( D3DSTENCILOP op )
		{
			switch(op)
			{
			case D3DSTENCILOP_KEEP :
				return SOP_Keep;

			case D3DSTENCILOP_ZERO :
				return  SOP_Zero;

			case D3DSTENCILOP_REPLACE :
				return SOP_Replace;

			case D3DSTENCILOP_INCRSAT :
				return SOP_Increment_Sat;

			case D3DSTENCILOP_DECRSAT :
				return SOP_Decrement_Sat ;

			case D3DSTENCILOP_INCR :
				return SOP_Increment_Wrap;

			case D3DSTENCILOP_DECR :
				return  SOP_Decrement_Wrap;

			case D3DSTENCILOP_INVERT :
				return SOP_Invert;

			}
			
			assert(false);
			return SOP_Keep;
		}

		D3DSTENCILOP  D3D9Mapping::Mapping( StencilOperation op )
		{
			switch(op)
			{
			case SOP_Keep:
				return D3DSTENCILOP_KEEP;

			case SOP_Zero:
				return  D3DSTENCILOP_ZERO;

			case SOP_Replace:
				return D3DSTENCILOP_REPLACE;
			
			case SOP_Increment_Sat:
				return D3DSTENCILOP_INCRSAT;

			case SOP_Decrement_Sat:
				return D3DSTENCILOP_DECRSAT;

			case SOP_Increment_Wrap:
				return D3DSTENCILOP_INCR;

			case SOP_Decrement_Wrap:
				return D3DSTENCILOP_DECR ;

			case SOP_Invert:
				return D3DSTENCILOP_INVERT;

			}

			return D3DSTENCILOP_FORCE_DWORD;
		}

		D3DTEXTUREADDRESS D3D9Mapping::Mapping( TextureAddressMode tm )
		{
			switch( tm )
			{
			case TAM_Wrap:
				return D3DTADDRESS_WRAP;

			case TAM_Mirror:
				return D3DTADDRESS_MIRROR;

			case TAM_Clamp:
				return D3DTADDRESS_CLAMP;

			case TAM_Mirror_Once:
				return D3DTADDRESS_MIRRORONCE;

			case TAM_Border:
				// 需要根据Device Cap 设置
				/*if (devCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
					return D3DTADDRESS_BORDER;
				else*/
					return D3DTADDRESS_CLAMP;

			}
			return D3DTADDRESS_FORCE_DWORD;
		}

		D3DBLEND D3D9Mapping::Mapping( BlendFactor bf )
		{
			switch(bf)
			{
			case BF_Zero:
				return D3DBLEND_ZERO ;

			case BF_One:
				return D3DBLEND_ONE;

			case BF_Src_Color:
				return D3DBLEND_SRCCOLOR;

			case BF_Inv_Src_Color:
				return D3DBLEND_INVSRCCOLOR;

			case BF_Src_Alpha:
				return D3DBLEND_SRCALPHA;
				 
			case BF_Inv_Src_Alpha:
				return D3DBLEND_INVSRCALPHA;

			case BF_Dst_Color:
				return D3DBLEND_DESTCOLOR;

			case BF_Inv_Dst_Color:
				return D3DBLEND_INVDESTCOLOR;
			
			case BF_Dst_Alpha:
				return D3DBLEND_DESTALPHA;

			case BF_Inv_Dst_Alpha:
				return D3DBLEND_INVDESTALPHA;

			case BF_Src_Alpha_Saturation:
				return D3DBLEND_SRCALPHASAT; 

			case BF_Both_Src_Alpha:
				return D3DBLEND_BOTHSRCALPHA;
			}
			
			// to keep compiler happy
			assert(false);
			return D3DBLEND_SRCCOLOR;
				 
		}

		D3DDECLTYPE D3D9Mapping::Mapping( VertexElementFormat vef )
		{
			switch(vef)
			{
			case VEF_Single:
				return D3DDECLTYPE_FLOAT1;
				
			case VEF_Vector2:
				return D3DDECLTYPE_FLOAT2;

			case VEF_Vector3:
				return D3DDECLTYPE_FLOAT3;

			case VEF_Vector4:
				return D3DDECLTYPE_FLOAT4;

			case VEF_Color:
				return D3DDECLTYPE_D3DCOLOR;

			case VEF_UByte4:
				return  D3DDECLTYPE_UBYTE4;

			case VEF_Short2:
				return D3DDECLTYPE_SHORT2;

			case VEF_Short4:
				return D3DDECLTYPE_SHORT4;

			case VEF_NormalizedShort2:
				return D3DDECLTYPE_SHORT2N;

			case VEF_NormalizedShort4:
				return D3DDECLTYPE_SHORT4N;
			
			case VEF_HalfVector2:
				return D3DDECLTYPE_FLOAT16_2;

			case VEF_HalfVector4:
				return D3DDECLTYPE_FLOAT16_4;
			}

			// to keep compiler happy
			assert(false);
			return D3DDECLTYPE_FLOAT3;
		}

		D3DDECLUSAGE D3D9Mapping::Mapping( VertexElementUsage veu )
		{
			switch(veu)
			{
			case VEU_Position:
				return D3DDECLUSAGE_POSITION;

			case VEU_BlendWeight:
				return D3DDECLUSAGE_BLENDWEIGHT;

			case VEU_BlendIndices:
				return D3DDECLUSAGE_BLENDINDICES;

			case VEU_Normal:
				return D3DDECLUSAGE_NORMAL;

			case VEU_Diffuse:
				return D3DDECLUSAGE_COLOR;

			case VEU_Specular:
				return  D3DDECLUSAGE_COLOR;
			
			case VEU_TextureCoordinate:
				return D3DDECLUSAGE_TEXCOORD;

			case VEU_Tangent:
				return D3DDECLUSAGE_TANGENT;

			case VEU_Binormal:
				return D3DDECLUSAGE_BINORMAL;		 
			}

			// to keep compiler happy
			assert(false);
			return D3DDECLUSAGE_POSITION;
		}

		D3DFORMAT D3D9Mapping::Mapping( IndexBufferType itype )
		{
			if (itype == IBT_Bit32)
			{
				return D3DFMT_INDEX32;
			}
			else
			{
				return D3DFMT_INDEX16;
			}
		}


		PixelFormat D3D9Mapping::GetClosetFormat( PixelFormat fmt )
		{
			if (Mapping(fmt) != D3DFMT_UNKNOWN)
			{
				return fmt;
			}
			switch(fmt)
			{
			case PF_B5G6R5:
				return PF_R5G6B5;
			case PF_B8G8R8:
				return PF_R8G8B8;
			case PF_B8G8R8A8:
				return PF_A8R8G8B8;
			case PF_Short_RGB:
				return PF_Short_RGBA;
			case PF_Float16_RGB:
				return PF_Float16_RGBA;
			case PF_Float32_RGB:
				return PF_Float32_RGBA;
			case PF_Unknown:
			default:
				return PF_A8R8G8B8;
			}
		}

		Engine::PixelFormat D3D9Mapping::MappingPixelFormt( D3DFORMAT d3dPF )
		{
			switch(d3dPF)
			{
			case D3DFMT_A8:
				return PF_Alpha8;
			case D3DFMT_L8:
				return PF_Luminance8;
			case D3DFMT_L16:
				return PF_Luminance16;
			case D3DFMT_A4L4:
				return PF_A4L4;
			case D3DFMT_A8L8:
				return PF_A8L8;	
			case D3DFMT_R3G3B2:
				return PF_R3G3B2;
			case D3DFMT_A1R5G5B5:
				return PF_A1R5G5B5;
			case D3DFMT_A4R4G4B4:
				return PF_A4R4G4B4;
			case D3DFMT_R5G6B5:
				return PF_R5G6B5;
			case D3DFMT_R8G8B8:
				return PF_R8G8B8;
			case D3DFMT_X8R8G8B8:
				return PF_X8R8G8B8;
			case D3DFMT_A8R8G8B8:
				return PF_A8R8G8B8;
			case D3DFMT_X8B8G8R8:
				return PF_X8B8G8R8;
			case D3DFMT_A8B8G8R8:
				return PF_A8B8G8R8;
			case D3DFMT_A2R10G10B10:
				return PF_A2R10G10B10;
			case D3DFMT_A2B10G10R10:
				return PF_A2B10G10R10;
			case D3DFMT_R16F:
				return PF_Float16_R;
			case D3DFMT_A16B16G16R16F:
				return PF_Float16_RGBA;
			case D3DFMT_R32F:
				return PF_Float32_R;
			case D3DFMT_G32R32F:
				return PF_Float32_GR;
			case D3DFMT_A32B32G32R32F:
				return PF_Float32_RGBA;
			case D3DFMT_G16R16F:
				return PF_Float16_GR;
			case D3DFMT_A16B16G16R16:
				return PF_Short_RGBA;
			case D3DFMT_G16R16:
				return PF_Short_GR;
			case D3DFMT_DXT1:
				return PF_DXT1;
			case D3DFMT_DXT2:
				return PF_DXT2;
			case D3DFMT_DXT3:
				return PF_DXT3;
			case D3DFMT_DXT4:
				return PF_DXT4;
			case D3DFMT_DXT5:
				return PF_DXT5;
			default:
				return PF_Unknown;
			}
		}

		
	}
}