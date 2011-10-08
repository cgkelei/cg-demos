#pragma once
#ifndef _D3D9Mapping__H
#define _D3D9Mapping__H

#include "D3D9Prerequisites.h"
#include "PixelFormat.h"
#include "GraphicsCommon.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class D3D9Mapping
		{
		public:

			static PixelFormat GetClosetFormat(PixelFormat fmt);
			
			/// <summary>
			/// utility method, convert D3D9 pixel format to Ogre pixel format
			/// </summary>
			static PixelFormat MappingPixelFormt(D3DFORMAT d3dPF);

			static D3DFORMAT Mapping(PixelFormat fmt);

			static D3DMULTISAMPLE_TYPE Mapping(uint32 SampleCount);

			static D3DSHADEMODE Mapping(ShadingType st);

			static D3DCMPFUNC Mapping(CompareFunction cf);
			static CompareFunction UnMapping(D3DCMPFUNC cf);


			static D3DCULL Mapping(CullingMode cm);
			static CullingMode UnMapping(D3DCULL cm);

			static D3DFILLMODE Mapping(PolygonMode fm);
			static PolygonMode UnMapping( D3DFILLMODE  fm );

			static D3DSTENCILOP Mapping(StencilOperation op);
			static StencilOperation UnMapping(D3DSTENCILOP op);

			static D3DTEXTUREADDRESS Mapping( TextureAddressMode tm );

			static D3DBLEND Mapping( BlendFactor bf );

			static D3DDECLTYPE Mapping( VertexElementFormat vef );

			static D3DDECLUSAGE Mapping( VertexElementUsage veu );

			/// <summary>
			/// Get index type
			/// </summary>
			static D3DFORMAT Mapping( IndexBufferType itype );
		};
		
	}
}



#endif // _D3D9Mapping__H