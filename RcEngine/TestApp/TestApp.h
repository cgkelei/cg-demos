#pragma once

#include "MainApp/Application.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/SimpleGeometry.h"
#include "Graphics/CameraControler.h"

#pragma comment(lib, "RcEngine.lib")

using namespace RcEngine;

class TestApp : public RcEngine::Application
{
public:
	TestApp(const String& config);
	~TestApp(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void CalculateFrameRate();

	void InitGUI();


private:
	shared_ptr<Effect> mEffect;
	EffectTechnique* mTechnique;
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Texture> mTexture;
	shared_ptr<SimpleBox> mBox;
	shared_ptr<SimpleTexturedQuad> mQuad;
	shared_ptr<Mesh> mDwarf;
	shared_ptr<Material> mDwarfMaterial;
	shared_ptr<SpriteBatch> mSpriteBatchFont;
	shared_ptr<SpriteBatch> mSpriteBatch;
	shared_ptr<Font> mFont;
	FPSCameraControler* mCameraControler;
	int mFramePerSecond;
	
	//ModelViewerCameraControler* mCameraControler;
};

