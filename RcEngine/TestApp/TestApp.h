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
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Texture> mTexture;
};

