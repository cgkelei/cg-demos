#pragma once
#ifndef _D3D9VertexDeclaration__H
#define _D3D9VertexDeclaration__H

//  [8/25/2011 hustruan]

#include "D3D9Prerequisites.h"
#include "VertexDeclaration.h"
#include "D3D9Resource.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9VertexDeclaration : public VertexDeclaration, public D3D9Resource
		{
		public:
			D3D9VertexDeclaration();
			~D3D9VertexDeclaration();

			const VertexElement& AddElement(uint16 source, uint32 offset, VertexElementFormat theType,
				VertexElementUsage semantic, uint16 index = 0);

			const VertexElement& InsertElement( uint32 atPosition, uint16 source, uint32 offset,
						VertexElementFormat theType, VertexElementUsage semantic, uint16 index = 0);

			void RemoveElement(uint32 elemIndex);
			void RemoveElement(VertexElementUsage semantic, uint16 index = 0);
			void RemoveAllElements(void);
			
			// Called immediately after the Direct3D device has been created.
			void NotifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

			// Called before the Direct3D device is going to be destroyed.
			void NotifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

			/** Gets the D3D9-specific vertex declaration. */
			IDirect3DVertexDeclaration9* GetD3DVertexDeclaration(void);
 
		private:
			void ReleaseDeclaration();

		private:
			typedef std::map<IDirect3DDevice9*, IDirect3DVertexDeclaration9*>	DeviceToDeclarationMap;
			typedef DeviceToDeclarationMap::iterator							DeviceToDeclarationIterator;

#pragma  warning(disable : 4251)
			DeviceToDeclarationMap		mMapDeviceToDeclaration;
#pragma  warning(default : 4251)


		};
	}
}



#endif