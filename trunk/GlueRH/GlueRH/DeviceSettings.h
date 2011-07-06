#ifndef  _DeviceSettings__H
#define _DeviceSettings__H

#include "Common.h"

namespace GlueRH
{

	struct Direct3D10Settings
	{
		/** Adapter ordinal of the Direct3D 10 device. See IDXGIFactory.EnumAdapters. 
    	 */
		int32 AdapterOrdinal;

		/** Driver type of the Direct3D 10 device. See D3D10_DRIVER_TYPE. 
		 */
		D3D10_DRIVER_TYPE DriverType;
		
		/** Output ordinal of the Direct3D 10 device. See IDXGIAdapter.EnumOutputs.
		 */
		int32 OutputOrdinal;
		
		/** Description of the swap chain.See IDXGIAdapter.EnumOutputs. 
		 */
		DXGI_SWAP_CHAIN_DESC SwapChainDescription;

		/** Flags used to create the Direct3D 10 device. See D3D10_CREATE_DEVICE_FLAG. 
		 */
		uint32 CreationFlags;

		/** Internal used for vertical sync. See IDXGISwapChain.Present. 
		 */
		int32 SyncInterval;
		
		/** Present flags. See IDXGISwapChain.Present. 
		 */
		uint32 PresentFlags;

		/** Format of the depth stencil. See DXGI_FORMAT. 
         */
		Format DepthStencilFormat;

		Direct3D10Settings() 
			: DriverType( D3D10_DRIVER_TYPE_HARDWARE ),
			  PresentFlags( 0 ),
			  SyncInterval( 1 ),
			  DepthStencilFormat( DXGI_FORMAT_UNKNOWN  )
		{

		}  
	};
	

	struct DeviceSettings
	{
		/** The adapter ordinal.
		 */
		int32 AdapterOrdinal;

		/** The type of the device.
		 */
		D3D10_DRIVER_TYPE DriverType;

		
		/** The refresh rate.
		 */
		int32 RefreshRate;

		/** The width of the back buffer. 
		 */
		int32 BackBufferWidth;

		/** The height of the back buffer.
		 */
	    int32 BackBufferHeight;
		
		/** The back buffer format.
		 */
	    DXGI_FORMAT BackBufferFormat;

		/** The back buffer count.
		 */
	    int32 BackBufferCount;

		/** indicating whether the device is windowed
		 */
		bool Windowed;

		/** Indicating whether VSync is enabled.
		 *  True if VSync is enabled; otherwise false
		 */
		bool EnableVSync;
	
		/** The multisample type.
		 */
		int32 MultisampleType;


		/** The multisample quality.
		 */
		int32 MultisampleQuality;

		/** The depth stencil format.
		 */
		Format DepthStencilFormat;

		/** The Direct3D10 specific settings.
		 */
		Direct3D10Settings Direct3D10;

		DeviceSettings() 
			: DriverType( D3D10_DRIVER_TYPE_HARDWARE ),
			  BackBufferFormat( DXGI_FORMAT_UNKNOWN ),
			  BackBufferCount( 1 ),
			  MultisampleType( 1 ),
			  MultisampleQuality( 0 ),
			  DepthStencilFormat( DXGI_FORMAT_UNKNOWN ),
			  Windowed( true ),
			  EnableVSync( true )
		{

		}
	
	};
}


#endif

