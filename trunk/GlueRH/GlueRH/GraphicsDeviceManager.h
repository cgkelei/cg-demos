#ifndef _GraphicsDeviceManager__H
#define _GraphicsDeviceManager__H

#include "Common.h"
#include "DeviceSettings.h"

namespace GlueRH
{
	/// <summary>
	/// Handles the configuration and management of the graphics device.
	/// </summary>
	class GraphicsDeviceManager
	{
	public:
		GraphicsDeviceManager( ApplicationPtr app );
		~GraphicsDeviceManager(void);

		bool IsWindowed() const { return mCurrentSettings.Windowed; }

		/**  Gets the width of the screen.
		 */
		int32 GetScreenWidth() const { return mCurrentSettings.BackBufferWidth; }

		/**  Gets the height of the screen.
		 */
		int32 GetScreenHeight() const { return mCurrentSettings.BackBufferHeight; }


		/** Ensures that the device is properly initialized and ready to render.
		 */
		bool EnsureDevice()
		{
			if ( mD3D10Device!= NULL )
				return true;

			return false;
		}

		/** Get DXGI Factory
		 */
		static IDXGIFactory* GetFactory() { return mFactory; }
		

	private:

		void Game_FrameStart(bool* cancel);
		void Game_FrameEnd();
		void Window_UserResized();
		void Window_MonitorChanged();

		void InitializeDevice();

		int32 GetAdapterOrdinal( HMONITOR mon );
		
		void UpdateDeviceInformation();

		void EnsureD3D10();

		void SetupDirect3D10Views();

		void ResizeDXGIBuffers( int width, int height, bool fullscreen );

	private:

		bool mIgnoreSizeChanges;
		bool mDeviceLost;

		bool mDoNotStoreBufferSize;
		bool mRenderingOccluded;

		int32 mFullscreenWindowWidth;
		int32 mFullscreenWindowHeight;
		int32 mWindowedWindowWidth;
		int32 mWindowedWindowHeight;
		WINDOWPLACEMENT m_windowedPlacement;
		uint64 mWindowedStyle;
		//bool mSavedTopmost;
	
		
		
		IDXGIFactory* mFactory;
		ID3D10Device* mD3D10Device;
		IDXGISwapChain* mSwapChain;

		DeviceSettings mCurrentSettings;

		
		ApplicationPtr mApplication;

	
	
	
	
	
	
	
	
	
	};
}

#endif



