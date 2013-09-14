#pragma once
#ifndef SponzaApp_h__
#define SponzaApp_h__

#include <MainApp/Application.h>
#include <Graphics/CameraControler.h>

using namespace RcEngine;

class SponzaApp : public Application
{
public:
	SponzaApp(const String& config);
	~SponzaApp(void);

protected:

	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void CalculateFrameRate();

	void InitGUI();
	void DrawUI();

protected:
	FPSCameraControler* mCameraControler;

	Label* mFPSLabel;
	int mFramePerSecond;
};

#endif // SponzaApp_h__
