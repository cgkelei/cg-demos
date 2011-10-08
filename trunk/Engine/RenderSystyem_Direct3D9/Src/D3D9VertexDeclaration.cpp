#include "D3D9VertexDeclaration.h"
#include "D3D9Mapping.h"
#include "D3D9RenderSystem.h"
#include "D3D9Util.h"
#include "EngineRoot.h"
#include "Exception.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		D3D9VertexDeclaration::D3D9VertexDeclaration()
		{

		}

		D3D9VertexDeclaration::~D3D9VertexDeclaration()
		{
			RemoveAllElements();
		}


		void D3D9VertexDeclaration::ReleaseDeclaration()
		{
			DeviceToDeclarationIterator it = mMapDeviceToDeclaration.begin();

			while (it != mMapDeviceToDeclaration.end())
			{
				SAFE_RELEASE(it->second);
				++it;
			}	
			mMapDeviceToDeclaration.clear();
		}

		
		const VertexElement& D3D9VertexDeclaration::AddElement( uint16 source, uint32 offset, VertexElementFormat theType, VertexElementUsage semantic, uint16 index /*= 0*/ )
		{
			ReleaseDeclaration();
			return VertexDeclaration::AddElement(source, offset, theType, semantic, index);
		}

		const VertexElement& D3D9VertexDeclaration::InsertElement( uint32 atPosition, uint16 source, uint32 offset, VertexElementFormat theType, VertexElementUsage semantic, uint16 index /*= 0*/ )
		{
			ReleaseDeclaration();   
			return VertexDeclaration::InsertElement(atPosition, source, offset, theType, semantic, index);
		}

		void D3D9VertexDeclaration::RemoveElement( uint32 elemIndex )
		{
			VertexDeclaration::RemoveElement(elemIndex);
			ReleaseDeclaration();   
		}

		void D3D9VertexDeclaration::RemoveElement( VertexElementUsage semantic, uint16 index /*= 0*/ )
		{
			VertexDeclaration::RemoveElement(semantic, index);
			ReleaseDeclaration();   
		}

		void D3D9VertexDeclaration::RemoveAllElements( void )
		{
			VertexDeclaration::RemoveAllElements();
			ReleaseDeclaration();   
		}

		void D3D9VertexDeclaration::NotifyOnDeviceCreate( IDirect3DDevice9* d3d9Device )
		{
				
		}

		void D3D9VertexDeclaration::NotifyOnDeviceDestroy( IDirect3DDevice9* d3d9Device )
		{
			DeviceToDeclarationIterator it = mMapDeviceToDeclaration.find(d3d9Device);

			if (it != mMapDeviceToDeclaration.end())	
			{
				SAFE_RELEASE(it->second);	
				mMapDeviceToDeclaration.erase(it);
			}
		}

		IDirect3DVertexDeclaration9* D3D9VertexDeclaration::GetD3DVertexDeclaration( void )
		{
			IDirect3DDevice9* pCurDevice   = D3D9RenderSystem::GetActiveD3D9Device();
			DeviceToDeclarationIterator it = mMapDeviceToDeclaration.find(pCurDevice);
			IDirect3DVertexDeclaration9* lpVertDecl = NULL;

			// Case we have to create the declaration for this device.
			if (it == mMapDeviceToDeclaration.end() || it->second == NULL)
			{
				D3DVERTEXELEMENT9* d3delems = new D3DVERTEXELEMENT9[mElementList.size() + 1];

				VertexElementList::const_iterator i, iend;
				unsigned int idx;
				iend = mElementList.end();
				for (idx = 0, i = mElementList.begin(); i != iend; ++i, ++idx)
				{
					d3delems[idx].Method = D3DDECLMETHOD_DEFAULT;
					d3delems[idx].Offset = static_cast<WORD>(i->GetOffset());
					d3delems[idx].Stream = i->GetSource();
					d3delems[idx].Type = D3D9Mapping::Mapping(i->GetType());
					d3delems[idx].Usage = D3D9Mapping::Mapping(i->GetUsage());
					// NB force index if colours since D3D uses the same usage for 
					// diffuse & specular
					if (i->GetUsage() == VEU_Specular)
					{
						d3delems[idx].UsageIndex = 1;
					}
					else if (i->GetUsage() == VEU_Diffuse)
					{
						d3delems[idx].UsageIndex = 0;
					}
					else
					{
						d3delems[idx].UsageIndex = static_cast<BYTE>(i->GetUsageIndex());
					}
				}
				// Add terminator
				d3delems[idx].Stream = 0xff;
				d3delems[idx].Offset = 0;
				d3delems[idx].Type = D3DDECLTYPE_UNUSED;
				d3delems[idx].Method = 0;
				d3delems[idx].Usage = 0;
				d3delems[idx].UsageIndex = 0;


				HRESULT hr = pCurDevice->CreateVertexDeclaration(d3delems, &lpVertDecl);

				if (FAILED(hr))
				{
					ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
						"Cannot create D3D9 vertex declaration: " + D3D9Util::GetErrorDescription(hr), 
						"Direct3D9VertexDeclaration::GetD3DVertexDeclaration");
				}

				delete [] d3delems;

				mMapDeviceToDeclaration[pCurDevice] = lpVertDecl;
			}

			// Declaration already exits.
			else
			{
				lpVertDecl = mMapDeviceToDeclaration[pCurDevice];
			}

			return lpVertDecl;
		}
	}
}