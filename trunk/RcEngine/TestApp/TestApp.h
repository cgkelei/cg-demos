#pragma once

#include "MainApp/Application.h"

#pragma comment(lib, "RcEngine.lib")

class TestApp : public RcEngine::Application
{
public:
	TestApp(void);
	~TestApp(void);

protected:
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);
};

