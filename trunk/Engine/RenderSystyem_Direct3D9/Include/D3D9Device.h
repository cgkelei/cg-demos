#pragma once
#ifndef _D3D9Device__H
#define _D3D9Device__H

#include "D3D9Prerequisites.h"


//  [8/10/2011 hustruan]

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9Device 
		{
			
			friend class D3D9DeviceManager;
			friend class D3D9RenderSystem;

		public:

			D3D9Device(D3D9DeviceManager* deviceManager, UINT adapterNumber, 
							HMONITOR hMonitor, D3DDEVTYPE devType, DWORD behaviorFlags);
			~D3D9Device();

			void AttachRenderWindow(D3D9RenderWindow* renderWindow);
			void DetachRenderWindow	(D3D9RenderWindow* renderWindow);

			bool Acquire();

			bool IsDeviceLost();

			void Release();		
			void Destroy();		
			
			void NotifyDeviceLost();

			bool Validate(D3D9RenderWindow* renderWindow);
			void Invalidate(D3D9RenderWindow* renderWindow);

			void Present(D3D9RenderWindow* renderWindow);

			
			IDirect3DDevice9*   GetD3D9Device();
			IDirect3DSurface9*	GetDepthBuffer();			
			IDirect3DSurface9*	GetBackBuffer();	
			D3DFORMAT			GetBackBufferFormat() const ;
			UINT				GetAdapterNumber() const	{ return mAdapterNumber; }
			D3DDEVTYPE			GetDeviceType() const		{ return mDeviceType; }
			const D3DCAPS9&	    GetD3D9DeviceCaps() const	{ return mD3D9DeviceCaps; }
			bool				IsAutoDepthStencil() const;
			uint32				GetLastPresentFrame	() const { return mLastPresentFrame; }

			
			
			
		private:
			void CreateD3D9Device();
			void ReleaseD3D9Device();
			void ReleaseRenderWindowResource();
			void UpdateRenderWindowPresentation();
			void ClearDeviceStreams();

			void AcquireRenderWindowResources();
		
			void SetupDeviceStates();

			void		ValidateBackBufferSize	(D3D9RenderWindow* renderWindow);
			bool		ValidateDisplayMonitor	(D3D9RenderWindow* renderWindow);
			bool		ValidateDeviceState		(D3D9RenderWindow* renderWindow);
			
			bool		Reset					();
			bool        Acquire(D3D9RenderWindow* renderWin);

		private:

			D3D9DeviceManager*				mpDeviceManager;			// The manager of this device instance.

			IDirect3DDevice9*				mpDevice;					// Will hold the device interface.				
			UINT							mAdapterNumber;				// The adapter that this device belongs to.	
			HMONITOR						mMonitor;					// The monitor that this device belongs to.
			D3DDEVTYPE						mDeviceType;				// Device type.	
			HWND							mFocusWindow;				// The focus window this device attached to.			
			DWORD							mBehaviorFlags;				// The behavior of this device.		
			
			D3DPRESENT_PARAMETERS			mPresentationParam;		// Presentation parameters which the device was created with. May be
			D3DCAPS9						mD3D9DeviceCaps;			// Device caps.	
			bool							mD3D9DeviceCapsValid;		// True if device caps initialized.				
			D3DDEVICE_CREATION_PARAMETERS	mCreationParams;			// Creation parameters.
			UINT							mLastPresentFrame;			// Last frame that this device present method called.
			bool							mDeviceLost;				// True if device entered lost state.
			IDirect3DSwapChain9* 			mSwapChain;					// Swap chain interface.
			IDirect3DSurface9*	 			mBackBuffer;				// The back buffer of the render window.
			IDirect3DSurface9*	 			mDepthBuffer;				// The depth buffer of the render window.
			bool							mAcquired;					// True if resources acquired.			
			
			D3D9RenderWindow*				mActiveRenderWindow;		
		};
	}
}





#endif // _D3D9Device__H