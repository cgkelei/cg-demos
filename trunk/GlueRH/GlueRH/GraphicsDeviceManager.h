#ifndef _GraphicsDeviceManager__H
#define _GraphicsDeviceManager__H

#include "Common.h"

namespace GlueRH
{
	/// <summary>
	/// Handles the configuration and management of the graphics device.
	/// </summary>
	class GraphicsDeviceManager
	{
	public:
		GraphicsDeviceManager(void);
		~GraphicsDeviceManager(void);


	private:

		void Game_FrameStart(bool* cancel);
		void Game_FrameEnd();
		void Window_UserResized();
		void Window_MonitorChanged();

		void InitializeDevice();

		int32 GetAdapterOrdinal(HMONITOR mon);
		
		void UpdateDeviceInformation();


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
	
	
	
	
	
	
	
	
	
	
	
	
	
	};
}

#endif



