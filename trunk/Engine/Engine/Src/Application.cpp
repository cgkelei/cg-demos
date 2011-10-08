#include "Application.h"
#include "WindowEventUtils.h"
#include "EngineRoot.h"
#include "Exception.h"
#include "RenderSettings.h"
#include "RenderSystem.h"
#include "RenderWindow.h"
#include "GameClock.h"
#include "GameTime.h"
#include "Camera.h"

namespace Engine
{
	
	Application::Application( void )
		: mRoot(NULL),
		  mCamera(NULL),
		  mWindow(NULL),
		  mIsExiting(false)

	{
		mGameClock = new GameClock();
		mRoot = new EngineRoot();

		mRoot->LoadPlugins();

		mRenderer = mRoot->GetRenderSystem();

		mWindow = mRoot->Initialize(true, "My Engine Window");

		//Register as a Window listener
		Engine::WindowEventUtilities::AddWindowEventListener(mWindow, this);

	}

	Application::~Application( void )
	{
		WindowClosed(mWindow);
		delete mRoot;
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

	void Application::Tick()
	{
		if(mIsExiting)
			return;

		if(!mWindow->IsActive())
			Sleep(static_cast<int>(mInactiveSleepTime));

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
			
			try
			{
				GameTime gt(mTargetElapsedTime, mTotalGameTime,
					elapsedRealTime,totalRealTime, mFramesPerSecond, mDrawRunningSlowly);

				Update(&gt);
			}
			catch(...)
			{
				mLastFrameElapsedGameTime += targetElapsedTime;
				mTotalGameTime += targetElapsedTime;
			}
			

			mLastFrameElapsedGameTime += targetElapsedTime;
			mTotalGameTime += targetElapsedTime;
		}

		{
			GameTime gt(mTargetElapsedTime, mTotalGameTime,
				elapsedRealTime,totalRealTime, mFramesPerSecond, mDrawRunningSlowly);
			DrawFrame(&gt );
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

	bool Application::FrameStarted( const FrameEvent& evt )
	{
	
		return true;
	}

	bool Application::FrameEnded( const FrameEvent& evt )
	{
		return true;
	}

	void Application::WindowResized( RenderWindow* rw )
	{
		unsigned int width, height, depth;
		int left, top;
		rw->GetMetrics(width, height, depth, left, top);
	}

	void Application::WindowClosed( RenderWindow* rw )
	{

	}


	void Application::DrawFrame( const GameTime* time )
	{
		Render(time);
		mLastFrameElapsedGameTime = 0;
		mLastFrameElapsedRealTime = 0;
		
	}
	
	bool Application::IsActive() const
	{
		return mWindow->IsActive();
	}

	void Application::Initialize()
	{
	
	}
}