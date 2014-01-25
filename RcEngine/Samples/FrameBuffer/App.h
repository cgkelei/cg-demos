#pragma once
#include <MainApp/Application.h>

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

protected:
	shared_ptr<FrameBuffer> mGbuffer;
};

