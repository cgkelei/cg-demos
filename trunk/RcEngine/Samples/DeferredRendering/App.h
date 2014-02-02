#pragma once
#include <MainApp/Application.h>
#include <Graphics/Renderer.h>
#include <Graphics/CameraController1.h>

using namespace RcEngine;

class App : public Application
{
public:
	App(const String& config);
	~App(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void SetupLights();
	void InitGUI();
	void CalculateFrameRate();

	void WindowResize(uint32_t width, uint32_t height);


protected:
	Renderer* mSceneRender;
	shared_ptr<Pipeline> mDeferredPipeline;
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;

	Label* mLabel;
	UIWindow* mWindow;
};

