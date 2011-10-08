#include "D3D9Driver.h"
#include "D3D9VideoModeList.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		D3D9Driver::D3D9Driver()
		{
			mAdapterNumber	= 0;
			ZeroMemory( &mD3D9DeviceCaps, sizeof(mD3D9DeviceCaps) );
			ZeroMemory( &mAdapterIdentifier, sizeof(mAdapterIdentifier) );
			ZeroMemory( &mDesktopDisplayMode, sizeof(mDesktopDisplayMode) );		
			mpVideoModeList = NULL;				
		}


		D3D9Driver::D3D9Driver( unsigned int adapterNumber, const D3DCAPS9& deviceCaps,
			const D3DADAPTER_IDENTIFIER9& adapterIdentifer,
			const D3DDISPLAYMODE& desktopDisplayMode)
			: mAdapterNumber(adapterNumber),
			mAdapterIdentifier(adapterIdentifer),
			mD3D9DeviceCaps(deviceCaps),
			mDesktopDisplayMode(desktopDisplayMode),
			mpVideoModeList(NULL)
		{

		}


		D3D9Driver::D3D9Driver( const D3D9Driver &ob )
		{
			mAdapterNumber		= ob.mAdapterNumber;
			mD3D9DeviceCaps		= ob.mD3D9DeviceCaps;
			mAdapterIdentifier	= ob.mAdapterIdentifier;
			mDesktopDisplayMode = ob.mDesktopDisplayMode;
			mpVideoModeList		= NULL;				
		}

		D3D9Driver::~D3D9Driver()
		{
			SAFE_DELETE(mpVideoModeList);
		}

		String D3D9Driver::DriverDescription() const
		{
			StringStream str;
			str << "Monitor-" << (mAdapterNumber+1) << "-" << mAdapterIdentifier.Description;

			return str.str();

			/*String driverDescription(str.str());
			StringUtil::trim(driverDescription);
			return  driverDescription;*/
		}


		D3D9VideoModeList* D3D9Driver::GetVideoModeList()
		{
			if( !mpVideoModeList )
				mpVideoModeList = new D3D9VideoModeList( this );

			return mpVideoModeList;
		}
	}
}