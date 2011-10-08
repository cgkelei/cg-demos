#include "D3D9VideoModeList.h"
#include "D3D9Driver.h"
#include "Exception.h"
#include "D3D9RenderSystem.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		D3D9VideoModeList::D3D9VideoModeList(D3D9Driver* driver)
		{
			if( NULL == driver )
				ENGINE_EXCEPT( Exception::ERR_INVALIDPARAMS, "pDriver parameter is NULL", "D3D9VideoModeList::D3D9VideoModeList" );

			mpDriver = driver;
			Enumerate();
		}

		D3D9VideoModeList::~D3D9VideoModeList(void)
		{
			mpDriver = NULL;
			mModeList.clear();
		}

		bool D3D9VideoModeList::Enumerate()
		{
			UINT iMode;
			IDirect3D9* pD3D = D3D9RenderSystem::GetDirect3D9();
			UINT adapter = mpDriver->GetAdapterNumber();

			for(iMode = 0; iMode < pD3D->GetAdapterModeCount( adapter, D3DFMT_R5G6B5); iMode++)
			{
				D3DDISPLAYMODE displayMode;
				ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODE));
				pD3D->EnumAdapterModes( adapter, D3DFMT_R5G6B5, iMode, &displayMode );

				// Filter out low-resolutions
				if( displayMode.Width < 640 || displayMode.Height < 400 )
					continue;

				// Check to see if it is already in the list (to filter out refresh rates)
				bool found = false;
				std::vector<D3D9VideoMode>::iterator it;
				for( it = mModeList.begin(); it != mModeList.end(); it++ )
				{
					D3DDISPLAYMODE oldDisp = it->GetDisplayMode();
					if( oldDisp.Width == displayMode.Width &&
						oldDisp.Height == displayMode.Height &&
						oldDisp.Format == displayMode.Format )
					{
						// Check refresh rate and favour higher if poss
						if (oldDisp.RefreshRate < displayMode.RefreshRate)
							it->IncreaseRefreshRate(displayMode.RefreshRate);
						found = true;
						break;
					}
				}

				if( !found )
					mModeList.push_back( D3D9VideoMode( displayMode ) );

			}
			
			for(iMode = 0; iMode < pD3D->GetAdapterModeCount( adapter, D3DFMT_X8R8G8B8); iMode++)
			{
				D3DDISPLAYMODE displayMode;
				ZeroMemory(&displayMode, sizeof(D3DDISPLAYMODE));
				pD3D->EnumAdapterModes( adapter, D3DFMT_R5G6B5, iMode, &displayMode );

				// Filter out low-resolutions
				if( displayMode.Width < 640 || displayMode.Height < 400 )
					continue;

				// Check to see if it is already in the list (to filter out refresh rates)
				bool found = false;
				std::vector<D3D9VideoMode>::iterator it;
				for( it = mModeList.begin(); it != mModeList.end(); it++ )
				{
					D3DDISPLAYMODE oldDisp = it->GetDisplayMode();
					if( oldDisp.Width == displayMode.Width &&
						oldDisp.Height == displayMode.Height &&
						oldDisp.Format == displayMode.Format )
					{
						// Check refresh rate and favour higher if poss
						if (oldDisp.RefreshRate < displayMode.RefreshRate)
							it->IncreaseRefreshRate(displayMode.RefreshRate);
						found = true;
						break;
					}
				}

				if( !found )
					mModeList.push_back( D3D9VideoMode( displayMode ) );

			}

			return true;
	
		}

		Engine::uint32 D3D9VideoModeList::Count()
		{
			return static_cast<uint32>(mModeList.size());
		}

		D3D9VideoMode* D3D9VideoModeList::Item( uint32 index )
		{
			return &mModeList.at(index);
		}

		D3D9VideoMode* D3D9VideoModeList::Item( const String &name )
		{
			std::vector<D3D9VideoMode>::iterator it = mModeList.begin();
			if (it == mModeList.end())
				return NULL;

			for (;it != mModeList.end(); ++it)
			{
				if (it->GetDescription() == name)
					return &(*it);
			}

			return NULL;
		}
	}
}