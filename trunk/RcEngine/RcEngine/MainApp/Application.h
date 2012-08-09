#ifndef Application_h__
#define Application_h__

#include <Core/Prerequisites.h>
#include <Core/Timer.h>
#include <Graphics/RenderSettings.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Camera.h>


namespace RcEngine {

class Window;

class _ApiExport Application
{
public:
	Application(void);
	virtual ~Application(void);

	/**
	 * This method will create reader device with render settings and then 
	 * load all modules, create render window
	 */
	virtual void Create(); 

	/**
	 * This method will release all modules.
	 */
	virtual void Release();

	/// <summary>
	// Call this method to initialize the game, begin running the game loop
	// and start processing events for the game.
	/// </summary>
	void RunGame();

	Window* GetMainWindow() { return mMainWindow; }

protected:

	//Called after the Game and GraphicsDevice are created, but before LoadContent.
	virtual void Initialize() = 0;

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
	virtual void Render() = 0;

	/// <summary>
	/// Allows the game to perform logic processing
	/// </summary>
	/// <param name ="timer"> The time passed since the last update </param>
	virtual void Update(float deltaTime) = 0;

private:
	void Tick();
	void LoadAllModules();
	void UnloadAllModules();

	void ReadConfiguration();

	void Window_ApplicationActivated();
	void Window_ApplicationDeactivated();
	void Window_Suspend();
	void Window_Resume();
	void Window_Paint();

protected:

	Window* mMainWindow;

	bool mActice;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	Timer mTimer;

	Render::RenderSettings mSettings;

	// in case multiple threads are used
	volatile bool mIsRunning;

	std::string mConfigFile; 

};

} // Namespace RcEngine

#endif // Application_h__