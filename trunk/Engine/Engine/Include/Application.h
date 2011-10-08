#pragma once
#ifndef _Application__H
#define _Application__H

#include "Prerequisites.h"
#include "Listeners.h"


namespace Engine
{
	class _ApiExport Application : public WindowEventListener, public FrameListener
	{
	public:
		Application(void);
		virtual ~Application(void);
		
		/// <summary>
		/// Gets a value indicating whether this is active.
		/// </summary>
		/// <value><c>true</c> if active; otherwise, <c>false</c>.<alue>
		bool IsActive() const;

		/// <summary>
		/// Gets a value indicating whether this is exiting.
		/// </summary>
		/// <value><c>true</c> if exiting; otherwise, <c>false</c>.<alue>
		bool IsExiting() const ;
	
		/// <summary>
		// Call this method to initialize the game, begin running the game loop
		// and start processing events for the game.
		/// </summary>
		void Run();
		
		/// <summary>
		// Performs one complete frame for the game
		// Updates the game's clock and calls Update and Draw.
		/// </summary>
		void Tick();
	
	protected:
		/// <summary>
		// Initializes the game
		// Called after the Application and RenderSystem are created, but before LoadContent.
		/// </summary>
		virtual void Initialize();
		
		/// <summary>
		///  Allows a Game to attempt to free resources and perform other cleanup operations 
		///  before garbage collection reclaims the Game.
		/// </summary>
		virtual void Finalize() = 0;	

		/// <summary>
		// Load Graphical Resources
		// Called when graphics resources need to be loaded.
		/// </summary>
		virtual void LoadContent() = 0;

		/// <summary>
		// UnLoad Graphical Resources
		// Called when graphics resources need to be unloaded. 
		// Override this method to unload any game-specific graphics resources.
		/// </summary>
		virtual void UnloadContent() = 0;
		
		/// <summary>
		// Called when a frame is ready to be draw
		/// </summary>
		/// <param name ="timer"> The time passed since the last update </param>
		virtual void Render(const GameTime* timer) = 0;
		
		/// <summary>
		/// Allows the game to perform logic processing
		/// </summary>
		/// <param name ="timer"> The time passed since the last update </param>
		virtual void Update(const GameTime* timer) = 0;

		/// <summary>
		/// Starts the drawing of a frame. This method is followed by calls to Draw and EndDraw.
		/// </summary>
		virtual bool FrameStarted(const FrameEvent& evt);

		/// <summary>
		/// Ends the drawing of a frame. This method is preceeded by calls to Draw and BeginDraw.
		/// </summary>
		virtual bool FrameEnded(const FrameEvent& evt);

		/// <summary>
		/// Overrode WindowEventListener, listen to window event
		/// </summary>
		virtual void WindowResized(RenderWindow* rw);

		/// <summary>
		/// Overrode WindowEventListener, listen to window event
		/// </summary>
		virtual void WindowClosed(RenderWindow* rw);

	private:

		void DrawFrame( const GameTime* time );

	protected:
		
		EngineRoot*   mRoot;
		RenderWindow* mWindow;
		Camera*       mCamera;
		Viewport*	  mViewport;
		RenderSystem* mRenderer;

		bool mIsExiting;
	
		/// <summary>
		/// Config file path
		/// </summary>
		String		  mContexCfg;

		GameClock* mGameClock; 

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






	};
}



#endif // _Application__H