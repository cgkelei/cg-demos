#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/RenderEffect.h"
#include "Graphics/RenderPatameter.h"
#include "Graphics/Texture.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"

using namespace RcEngine::Math;

TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

struct TextureDiffuseVertex
{
	Vector3f Pos;
	Vector4f Color;
	Vector2f Tex;
};

struct TextureVertex
{
	Vector3f Pos;
	Vector2f Tex;
};

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetRenderFactory();

	TextureVertex vertices[] = 
	{
		{ Vector3f( -5.0f, -5.0f, 0.0f ), Vector2f(0.0f, 0.0f) },
		{ Vector3f(  5.0f, -5.0f, 0.0f ), Vector2f(1.0f, 0.0f) },
		{ Vector3f(  5.0f,  5.0f, 0.0f ), Vector2f(1.0f, 1.0f) },
		{ Vector3f( -5.0f,  5.0f, 0.0f ), Vector2f(0.0f, 1.0f) },
	};

	ElementInitData initData;
	initData.pData = vertices;
	initData.rowPitch = sizeof(vertices);

	mVertexBuffer = factory->CreateVertexBuffer(BU_Static, BA_Read_Only, &initData);

	uint16 indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	initData.rowPitch = sizeof(indices);
	initData.pData = indices;
	mIndexBuffer =factory->CreateIndexBuffer(BU_Static, BA_Read_Only, &initData);

	VertexDeclarationDesc vdsc[2] = {
		{ 0, VEF_Vector3,  VEU_Position, 0 },
		{ sizeof(Vector3f), VEF_Vector2, VEU_TextureCoordinate, 0 }
	};
	mVertexDecl = factory->CreateVertexDeclaration(vdsc, 2);

	mTexture = factory->CreateTextureFromFile("../Media/1.png", EAH_GPU_Read);

	mCamera->SetViewParams(Vector3f(0, 0, 30), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
	mCamera->SetProjectionParams(MathUtil::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 100.0f );

	mEffect = factory->CreateEffectFromFile("default", "../Media/Effects/default.cgfx");
	mTechnique = mEffect->GetTechniqueByName("LeftHandInOpenGL");
}

void TestApp::UnloadContent()
{
	BoundingSpheref sphere;
}

using namespace RcEngine::Render;

void TestApp::Render()
{
	mRenderDevice->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(1, 1, 1, 1), 1.0f, 0);

	const Matrix4f& ViewMatrix = mCamera->GetViewMatrix();
	const Matrix4f& ProjMatrix = mCamera->GetProjectionMatrix();


	Matrix4f WVP = ViewMatrix * ProjMatrix;

	RenderParameter* wvpPara = mEffect->GetParameterBySemantic("WorldViewProjection");
	RenderParameter* texPara = mEffect->GetParameterByName("texSamp");

	wvpPara->SetValue(WVP);
	texPara->SetValue(mTexture);

	RenderOperation rop;
	RenderOperation::StreamUnit streamUint;

	streamUint.Stream = mVertexBuffer;
	streamUint.VertexDecl = mVertexDecl;
	streamUint.Type = RenderOperation::ST_Geometry;

	rop.mVertexStreams.push_back(streamUint);

	rop.mIndexBuffer = mIndexBuffer;
	rop.mIndexType = IBT_Bit16;

	mRenderDevice->Draw(*mTechnique, rop);


	mRenderDevice->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{

}


int main()
{
	TestApp* app = new TestApp();
	app->RunGame();
	return 0;
}