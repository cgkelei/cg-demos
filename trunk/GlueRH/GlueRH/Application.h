#ifndef _Application__H
#define _Application__H

#include "Common.h"
#include "RenderSettings.h"
#include "GameTimer.h"

namespace GlueRH
{
	class Application
	{
	public:

		virtual void Initialize() = 0;
		virtual void LoadContent() = 0;
		virtual void UnloadContent() = 0;

		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceReset() = 0;

		virtual void Render(const GameTimer* const time) = 0;
		virtual void Update(const GameTimer* const time) = 0;

		void Run();
		
		virtual void OnResize( int32 width, int32 height );
		

	private:

		void OnActived( bool active );
		void OnPaint();

	protected:
		Application( const std::wstring&  name );
		virtual ~Application(void);
		virtual bool OnFrameStart();
		virtual void OnFrameEnd();

	public:

		virtual void Release();
		virtual void Create();

		void Pause() { mActive = false; }
		void UnPause() { mActive = true; }

		RenderDevicePtr GetRenderDevice() const { return mRenderDevice; }
		bool IsActive() const { return mActive; }

		static Application* GetApplication() { return m_pGlobalApp; }
		
			
	protected:

		virtual void InitMainWindow();
		virtual void InitRenderDevice();
		

	protected:

		std::wstring mTitle; // Window title name
		bool mActive;        // Is the application active?
		
		RenderDevicePtr mRenderDevice;

	
		static Application*	m_pGlobalApp;

	};

	/**
	 * This function is used to get global application instance
	 */
	inline Application* ApplicationInstance() 
	{
		return Application::GetApplication();
	}

}

#endif

