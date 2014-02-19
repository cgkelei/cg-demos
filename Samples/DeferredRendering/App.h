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

	void MouseButtonPress();


protected:
	Renderer* mSceneRender;
	shared_ptr<Pipeline> mDeferredPipeline;
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;

	Label* mLabel;
	UIWindow* mWindow;

	shared_ptr<RenderOperation> mSphere;
	shared_ptr<RenderOperation> mCone;
	shared_ptr<Material> mMaterial;

	Light* mDirLight;
	Light* mPointLight;
	Light* mSpotLight;

	std::vector<Light*> mSpotLights;
};

