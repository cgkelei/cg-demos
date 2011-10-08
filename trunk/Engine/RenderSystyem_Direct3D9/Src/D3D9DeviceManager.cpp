#include "D3D9DeviceManager.h"
#include "D3D9RenderWindow.h"
#include "D3D9Device.h"
#include "D3D9RenderSystem.h"
#include "D3D9DriverList.h"
#include "Exception.h"
#include "EngineRoot.h"
#include "LogManager.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		D3D9DeviceManager::D3D9DeviceManager()
			: mActiveDevice(NULL),
			 mActiveRenderWindowDevice(NULL)
		{

		}

		D3D9DeviceManager::~D3D9DeviceManager()
		{

		}

		void D3D9DeviceManager::SetActiveDevice( D3D9Device* device )
		{
			if (mActiveDevice != device)
			{
				mActiveDevice = device;

				D3D9RenderSystem*	renderSystem = static_cast<D3D9RenderSystem*>(EngineRoot::GetSingleton().GetRenderSystem());
				D3D9DriverList*		driverList	 = renderSystem->GetDirect3DDrivers();

				// Update the active driver member.
				for (uint32 i=0; i < driverList->Count(); ++i)
				{
					D3D9Driver* currDriver = driverList->Item(i);
					if (currDriver->GetAdapterNumber() == mActiveDevice->GetAdapterNumber())
					{
						renderSystem->mActiveD3DDriver = currDriver;
						break;
					}				
				}	

				// Invalidate active view port.
				renderSystem->mActiveViewport = NULL;
			}						
		}

		D3D9Device* D3D9DeviceManager::GetActiveDevice()
		{
			if (mActiveDevice == NULL)
			{
				ENGINE_EXCEPT( Exception::ERR_INVALIDPARAMS, 
					"Current active device is NULL !!!", 
					"D3D9RenderSystem::getActiveDevice" );
			}

			return mActiveDevice;		
		}

		uint32 D3D9DeviceManager::GetDeviceCount()
		{
			return static_cast<uint32>(mRenderDevices.size());
		}
		
		D3D9Device* D3D9DeviceManager::GetDevice( uint32 index )
		{
			return mRenderDevices.at(index);
		}


		void D3D9DeviceManager::NotifyOnDeviceDestroy( D3D9Device* device )
		{

		}

		D3D9Device* D3D9DeviceManager::GetDeviceFromD3D9Device( IDirect3DDevice9* d3d9Device )
		{
			DeviceIterator itDevice = mRenderDevices.begin();
			while (itDevice != mRenderDevices.end())
			{			
				if ((*itDevice)->GetD3D9Device() == d3d9Device)
				{					
					return *itDevice;
				}												
				++itDevice;
			}

			return NULL;
		}

		void D3D9DeviceManager::LinkRenderWindow( D3D9RenderWindow* renderWindow )
		{
			D3D9Device* renderDevice;

			// Detach from previous device.
			renderDevice = renderWindow->GetDevice();		
			if (renderDevice != NULL)		
				renderDevice->DetachRenderWindow(renderWindow);						

			D3D9RenderWindowList renderWindowsGroup;

			// Select new device for this window.		
			renderDevice = SelectDevice(renderWindow, renderWindowsGroup);

			// Link the windows group to the new device.
			for (uint32 i = 0; i < renderWindowsGroup.size(); ++i)
			{
				D3D9RenderWindow* currWindow = renderWindowsGroup[i];

				currWindow->SetDevice(renderDevice);
				renderDevice->AttachRenderWindow(currWindow);
			}

			renderDevice->Acquire();
			if (mActiveDevice == NULL)			
				SetActiveDevice(renderDevice);		
		}

		D3D9Device* D3D9DeviceManager::SelectDevice( D3D9RenderWindow* renderWindow, D3D9RenderWindowList& renderWindowsGroup )
		{
			D3D9RenderSystem*	renderSystem = static_cast<D3D9RenderSystem*>(EngineRoot::GetSingleton().GetRenderSystem());
			D3D9Device*			renderDevice	 = NULL;	
			IDirect3D9*			direct3D9	     = D3D9RenderSystem::GetDirect3D9();
			UINT				nAdapterOrdinal  = D3DADAPTER_DEFAULT;
			D3DDEVTYPE			devType			 = D3DDEVTYPE_HAL;						
			DWORD				extraFlags		 = 0;					
			D3D9DriverList*		driverList = renderSystem->GetDirect3DDrivers();


			// Default group includes at least the given render window.
			renderWindowsGroup.push_back(renderWindow);

			renderSystem->mActiveD3DDriver = FindDriver(renderWindow);
			nAdapterOrdinal = renderSystem->mActiveD3DDriver->GetAdapterNumber();
			
			
			// Try to find a matching device from current device list.
			if (renderDevice == NULL)
			{
				for (size_t i = 0; i < mRenderDevices.size(); ++i)
				{
					D3D9Device* currDevice = mRenderDevices[i];

					if (currDevice->GetAdapterNumber() == nAdapterOrdinal &&
						currDevice->GetDeviceType() == devType)
					{
						renderDevice = currDevice;
						break;
					}			
				}
			}

			// No matching device found -> try reference device type (might have been 
			// previously created as a fallback, but don't change devType because HAL
			// should be preferred on creation)
			if (renderDevice == NULL)
			{
				for (size_t i = 0; i < mRenderDevices.size(); ++i)
				{
					D3D9Device* currDevice = mRenderDevices[i];

					if (currDevice->GetAdapterNumber() == nAdapterOrdinal &&
						currDevice->GetDeviceType() == D3DDEVTYPE_REF)
					{
						renderDevice = currDevice;
						break;
					}			
				}
			}


			// No matching device found -> create new one.
			if (renderDevice == NULL)
			{
				renderDevice = new D3D9Device(this, nAdapterOrdinal, direct3D9->GetAdapterMonitor(nAdapterOrdinal), devType, extraFlags);
				mRenderDevices.push_back(renderDevice);
				if (mActiveDevice == NULL)			
					SetActiveDevice(renderDevice);											
			}				

			return renderDevice;		


		}

		D3D9Driver* D3D9DeviceManager::FindDriver( D3D9RenderWindow* renderWindow )
		{
			D3D9RenderSystem*	renderSystem = static_cast<D3D9RenderSystem*>(EngineRoot::GetSingleton().GetRenderSystem());
			IDirect3D9*				direct3D9	     = D3D9RenderSystem::GetDirect3D9();
			UINT					nAdapterOrdinal  = D3DADAPTER_DEFAULT;						
			HMONITOR				hRenderWindowMonitor = NULL;			
			D3D9DriverList*			driverList = renderSystem->GetDirect3DDrivers();

			// Find the monitor this render window belongs to.
			hRenderWindowMonitor = MonitorFromWindow(renderWindow->GetWindowHandle(), MONITOR_DEFAULTTONEAREST);


			// Find the matching driver using window monitor handle.
			for (uint32 i = 0; i < driverList->Count(); ++i)
			{
				D3D9Driver* currDriver       = driverList->Item(i);
				HMONITOR hCurrAdpaterMonitor = direct3D9->GetAdapterMonitor(currDriver->GetAdapterNumber());

				if (hCurrAdpaterMonitor == hRenderWindowMonitor)
				{
					return currDriver;				
				}
			}

			return NULL;
		}

		void D3D9DeviceManager::SetActiveRenderTargetDevice( D3D9Device* device )
		{
			mActiveRenderWindowDevice = device;
			if (mActiveRenderWindowDevice != NULL)		
				SetActiveDevice(mActiveRenderWindowDevice);		
		}
	}
}