#pragma once
#ifndef _D3D9RenderWindow__H
#define _D3D9RenderWindow__H

#include "D3D9Prerequisites.h"
#include "RenderWindow.h"

namespace Engine
{

	namespace RenderSystem_Direct3D9
	{

		class _D3D9Export D3D9RenderWindow : public RenderWindow
		{
		public:
			D3D9RenderWindow(HINSTANCE hInst, const String& name, const RenderSettings& settings);
			~D3D9RenderWindow();

			void Initialize();
			void Destroy(void);

			void Resize(uint32 width, uint32 height);
			void Reposition(uint32 left, uint32 top);
			void WindowMovedOrResized();
			void SetFullscreen(bool fullScreen);

			bool RequiresTextureFlipping() const { return false; }
			void SwapBuffers( bool waitForVSync = true );

			bool IsActive() const; 
			bool IsVSync() const  { return mVSync; }
			bool IsClosed() const { return mClosed; }
			bool IsDepthBuffered() const  { return mIsDepthBuffered; }
			bool GetSwitchingFullscreen() const { return mSwitchingFullscreen; }
				
			
			/// @copydoc RenderTarget::_beginUpdate
			void BeginUpdate();

			/// @copydoc RenderTarget::_endUpdate
			void EndUpdate();

			/// Build the presentation parameters used with this window
			void BuildPresentParameters	(D3DPRESENT_PARAMETERS* presentParams);
			
			void SetDevice(D3D9Device* device)	{ mDevice = device;  mDeviceValid = false; }
			D3D9Device* GetDevice()			{ return mDevice; }
			
			HWND GetWindowHandle() const { return mHWnd; }	

		private:
			void AdjustWindow(uint32 clientWidth, uint32 clientHeight,DWORD style, uint32* winWidth, uint32* winHeight);
			void UpdateWindowRect();
			void FinishSwitchingFullscreen();


		protected:
			
			HINSTANCE					mInstance;				// Process instance
			D3D9Device* 				mDevice;				// D3D9 device wrapper class.
			bool						mDeviceValid;			// Device was validation succeeded.
			HWND						mHWnd;					// Win32 Window handle		
			bool						mClosed;				// Is this window destroyed.		
			bool						mSwitchingFullscreen;	// Are we switching from fullscreen to windowed or vice versa		

			// For windowed mode, the refresh rate must be 0. 
			// For full-screen mode, the refresh rate is one of the refresh rates returned by EnumAdapterModes. 
			UINT						mDisplayFrequency;		// Display frequency.

			bool						mVSync;					// Use vertical sync or not.

			// The maximum rate at which the swap chain's back buffers can be presented to the front buffer
			uint32						mVSyncInterval;		

			bool						mUseNVPerfHUD;			// Use NV Perf HUD.


			DWORD						mStyle;					// Window style currently used for this window.

		};
	}
}


#endif // _D3D9RenderWindow__H