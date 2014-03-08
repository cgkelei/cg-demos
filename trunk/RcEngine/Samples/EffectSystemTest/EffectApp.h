#pragma once
#include <MainApp/Application.h>
#include <Graphics/Renderer.h>
#include <Graphics/CameraController1.h>

using namespace RcEngine;

class EffectApp : public Application 
{
public:
	EffectApp(const String& config);
	~EffectApp(void);

protected:
	void Initialize();
	void InitGUI();
	void LoadContent();
	void UnloadContent() {}
	void Update(float deltaTime);
	void Render();
	void CalculateFrameRate();

};

