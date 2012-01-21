#pragma once

#include "MainApp/Application.h"
#include "Graphics/VertexDeclaration.h"

#pragma comment(lib, "RcEngine.lib")

using namespace RcEngine::Render;

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

private:
	RenderEffect* mEffect;
	RenderTechnique* mTechnique;
	GraphicBuffer* mVertexBuffer;
	GraphicBuffer* mIndexBuffer;
	VertexDeclaration mVertexDecl;
	Texture* mTexture;
};

