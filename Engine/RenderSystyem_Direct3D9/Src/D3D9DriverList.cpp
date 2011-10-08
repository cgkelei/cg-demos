#include "D3D9DriverList.h"
#include "LogManager.h"
#include "Exception.h"
#include "D3D9RenderSystem.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		

		D3D9DriverList::D3D9DriverList()
		{
			Enumerate();
		}

		D3D9DriverList::~D3D9DriverList()
		{
			mDriverList.clear();
		}

		Engine::uint32 D3D9DriverList::Count() const
		{
			return static_cast<uint32>( mDriverList.size());
		}

		D3D9Driver* D3D9DriverList::Item( uint32 index )
		{
			return &mDriverList.at( index );
		}

		D3D9Driver* D3D9DriverList::Item( const String &name )
		{
			std::vector<D3D9Driver>::iterator iter = mDriverList.begin();

			if (iter == mDriverList.end())
				return NULL;

			for (;iter != mDriverList.end(); ++iter)
			{
				if(iter->DriverDescription() == name)
					return &(*iter);
			}

			return NULL;
		}

		bool D3D9DriverList::Enumerate()
		{
			IDirect3D9* pD3D = D3D9RenderSystem::GetDirect3D9();
			assert( pD3D != NULL);

			UINT adapterCount = pD3D->GetAdapterCount();

			LogMessageStream << "D3D9: Driver Detection Starts" << std::endl;

			for(UINT iAdapter = 0; iAdapter < adapterCount; iAdapter++)
			{
				D3DADAPTER_IDENTIFIER9 adapterIdentifier;
				D3DDISPLAYMODE d3ddm;
				D3DCAPS9 d3dcaps9;

				pD3D->GetAdapterIdentifier(iAdapter, 0, &adapterIdentifier);
				pD3D->GetAdapterDisplayMode(iAdapter, &d3ddm);
				pD3D->GetDeviceCaps(iAdapter, D3DDEVTYPE_HAL, &d3dcaps9);

				mDriverList.push_back( D3D9Driver(iAdapter, d3dcaps9, adapterIdentifier, d3ddm));
			}

			LogMessageStream <<  "D3D9: Driver Detection Ends"  << std::endl;

			return TRUE;
		}

	}
}