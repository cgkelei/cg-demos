#include "Application.h"
#include "Window.h"
#include "GameClock.h"
#include "GameTimer.h"

namespace GlueRH
{
	Application* Application::m_pGlobalApp = NULL;
	
	HINSTANCE g_hInstance;
	

//////////////////////////////////////////////////////////////////////////
	Application::Application(const std::wstring&  name)
		: mTitle(name)
	{
		mGameClock = std::make_shared<GameClock>();

		mMainWindow = std::make_shared<Window>(mTitle, 0, 0, 640, 480);
		mMainWindow->EventApplicationActivated().bind( this, &Application::Window_ApplicationActivated );
		mMainWindow->EventApplicationDeactivated().bind( this, &Application::Window_ApplicationDeactivated );
		mMainWindow->EventPaint().bind( this, &Application::Window_Paint );
		mMainWindow->EventSuspend().bind( this, &Application::Window_Suspend );
		mMainWindow->EventResume().bind( this, &Application::Window_Resume );



	}


	Application::~Application(void)
	{
		
	}

	void Application::Run()
	{
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT)
		{			
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				Tick();
			}
		}
	}

	void Application::Exit()
	{
		// request the game to terminate
		mMainWindow->CloseWindow();
		mExiting = true;
	}

	void Application::Tick()
	{
		// if we are exiting, do nothing
		if( mExiting )
			return ;

		// if we are inactive, sleep for a bit
		if( !mActive )
			::Sleep( static_cast<int>(mInactiveSleepTime) );

		mGameClock->Step();

		float elapsedRealTime = (float)mGameClock->GetElapsedTime();
		float totalRealTime = (float)mGameClock->GetCurrentTime();

		mLastFrameElapsedRealTime += (float)mGameClock->GetElapsedTime();

		float elapsedAdjustedTime = mGameClock->GetElapsedAdjustedTime();
		if (elapsedAdjustedTime < 0)
			elapsedAdjustedTime = 0;

		if (mForceElapsedTimeToZero)
		{
			elapsedRealTime = 0;
			mLastFrameElapsedRealTime = elapsedAdjustedTime = 0;
			mForceElapsedTimeToZero = false;
		}

		mAccumulatedElapsedGameTime += elapsedAdjustedTime;

		float targetElapsedTime = mTargetElapsedTime;
		float ratio = mAccumulatedElapsedGameTime / mTargetElapsedTime;


		mAccumulatedElapsedGameTime = fmod(mAccumulatedElapsedGameTime, targetElapsedTime);
		mLastFrameElapsedGameTime = 0;

		if (ratio == 0)
			return;


		if (ratio > 1)
		{
			mUpdatesSinceRunningSlowly2 = mUpdatesSinceRunningSlowly1;
			mUpdatesSinceRunningSlowly1 = 0;
		}
		else
		{
			if (mUpdatesSinceRunningSlowly1 < MAXINT32)
				mUpdatesSinceRunningSlowly1++;
			if (mUpdatesSinceRunningSlowly2 < MAXINT32)
				mUpdatesSinceRunningSlowly2++;
		}

		mDrawRunningSlowly = mUpdatesSinceRunningSlowly2 < 20;

		// update until it's time to draw the next frame
		while ( ratio > 1 )
		{
			ratio -= 1;

			GameTimer gt(mTargetElapsedTime, mTotalGameTime,
				elapsedRealTime,totalRealTime, mFramesPerSecond, mDrawRunningSlowly);

			Update(gt);
			
			mLastFrameElapsedGameTime += targetElapsedTime;
			mTotalGameTime += targetElapsedTime;
		}

		{
			GameTimer gt(mTargetElapsedTime, mTotalGameTime,
				elapsedRealTime,totalRealTime, mFramesPerSecond, mDrawRunningSlowly);
			DrawFrame( gt );
		}


		// refresh the FPS counter once per second
		mLastUpdateFrame++;
		if (totalRealTime - mLastUpdateTime > 1.0f)
		{
			mFramesPerSecond = (float)mLastUpdateFrame / (float)(totalRealTime - mLastUpdateTime);
			mLastUpdateTime = totalRealTime;
			mLastUpdateFrame = 0;
		}
	}

	void Application::DrawFrame( const GameTimer& time )
	{

	}

	

	bool Application::OnFrameStart()
	{
		bool result = false;
		FrameStart( &result );
		return result;
	}

	void Application::OnFrameEnd()
	{
		FrameEnd();
	}

	void Application::Create()
	{
		mMainWindow->LoadWindow();
	}

	void Application::Release()
	{
	}

	void Application::Window_ApplicationActivated()
	{
		mActive = true;
	}

	void Application::Window_ApplicationDeactivated()
	{
		mActive = false;
	}

	void Application::Window_Suspend()
	{
		mGameClock->Suspend();
	}

	void Application::Window_Resume()
	{
		mGameClock->Resume();
	}

	void Application::Window_Paint()
	{

	}



	

	

}
