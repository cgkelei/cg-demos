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

		virtual void Render( const GameTimer& time ) = 0;
		virtual void Update( const GameTimer& time ) = 0;

		/// <summary>
		/// Performs one complete frame for the game.
		/// </summary>
		void Tick();

		/// <summary>
		/// Runs the game.
		/// </summary>
		void Run();
		
		/// <summary>
		/// Exits the game.
		/// </summary>
		void Exit();


		virtual void OnResize( int32 width, int32 height );
		
		/// <summary>
		/// Gets a value indicating whether this <see cref="Game"/> is exiting.
		/// </summary>
		/// <value><c>true</c> if exiting; otherwise, <c>false</c>.</value>
		bool IsExiting() const { return mExiting; }

		/// <summary>
		/// Gets or sets a value indicating whether this <see cref="Game"/> is active.
		/// </summary>
		/// <value><c>true</c> if active; otherwise, <c>false</c>.</value>
		bool IsActive() const { return mActive; }


		
		

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
		
		static Application* GetApplication() { return m_pGlobalApp; }
		
			
	protected:

		virtual void InitMainWindow();
		virtual void InitRenderDevice();
	
	private:

		void Window_ApplicationActivated();
		void Window_ApplicationDeactivated();
		void Window_Suspend();
		void Window_Resume();
		void Window_Paint();
			
		void DrawFrame( const GameTimer& time );

	private:

		GameClockPtr mGameClock; 
		GameTimerPtr mGameTimer;

		float mMaxElapsedTime;
		float mTotalGameTime;
		float mAccumulatedElapsedGameTime;
		float mLastFrameElapsedGameTime;
		float mLastFrameElapsedRealTime;
		float mTargetElapsedTime;
		float mInactiveSleepTime;
		int32 mUpdatesSinceRunningSlowly1;
		int32 mUpdatesSinceRunningSlowly2;
		bool mForceElapsedTimeToZero;
		bool mDrawRunningSlowly;
		uint64 mLastUpdateFrame;
		float mLastUpdateTime;
		float mFramesPerSecond;

		bool mExiting;
		bool mActive;

		std::wstring mTitle; 
		WindowPtr mMainWindow;
		RenderDevicePtr mRenderDevice;

		static Application*	m_pGlobalApp;
		/// <summary>
		/// Occurs when a drawing frame is about to start.
		/// </summary>
		CancellableEventHandler FrameStart;

		/// <summary>
		/// Occurs when a drawing frame ends.
		/// </summary>
		EventHandler FrameEnd;


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

