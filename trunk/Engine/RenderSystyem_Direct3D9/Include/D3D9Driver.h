#pragma once
#ifndef _D3D9Driver__H
#define _D3D9Driver__H

//  [8/13/2011 hustruan]

#include "D3D9Prerequisites.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		// 保存适配器信息，即安装在电脑上的显卡信息
		//////////////////////////////////////////////////////////////////////////
		class _D3D9Export D3D9Driver
		{
		public:
			D3D9Driver();
			D3D9Driver( const D3D9Driver &ob );	// Copy
			D3D9Driver( unsigned int adapterNumber, const D3DCAPS9& deviceCaps,
				const D3DADAPTER_IDENTIFIER9& adapterIdentifer, const D3DDISPLAYMODE& desktopDisplayMode);
			~D3D9Driver();

			const D3DCAPS9&					GetDeviceCaps		() const  { return mD3D9DeviceCaps; }
			uint32							GetAdapterNumber	() const  { return mAdapterNumber; }
			const D3DADAPTER_IDENTIFIER9&	GetAdapterIdentifier() const  { return mAdapterIdentifier; }
			const D3DDISPLAYMODE&			GetDesktopMode		() const  { return mDesktopDisplayMode; }
			String							DriverName			() const  { return String(mAdapterIdentifier.DeviceName); }
			
			
			String							DriverDescription	() const;
			D3D9VideoModeList*				GetVideoModeList	();


		private:

			// Adapter number( Ordinal number ) denoting the display adapter number
			// if installed more than one video card, adapter maybe larger than 0
			UINT mAdapterNumber;

			// Device caps.
			D3DCAPS9 mD3D9DeviceCaps;	

			// Adapter identifier
			D3DADAPTER_IDENTIFIER9	mAdapterIdentifier;

			// Desktop display mode.
			D3DDISPLAYMODE			mDesktopDisplayMode;

			// Video modes list.
			D3D9VideoModeList*		mpVideoModeList;	

		};

	}
}


#endif // _D3D9Driver__H