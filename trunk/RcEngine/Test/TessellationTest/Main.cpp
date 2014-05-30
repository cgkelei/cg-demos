#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Scene/Light.h>
#include <Math/MathUtil.h>
#include <Core/Profiler.h>

using namespace RcEngine;

class TesselationApp : public Application
{
public:
	TesselationApp(const String& config)
		: Application(config)
	{
		
	}

	virtual ~TesselationApp(void)
	{

	}

protected:
	void Initialize()
	{

	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		mBezierCurveEffect = resMan.GetResourceByName<Effect>(RT_Effect, "Tessellation.effect.xml", "General");
		
		float2 bezierControlPoints[] = { float2(-200, -200),  float2(-100, 180),  float2(150, 300),  float2(300, -100) };
		
		ElementInitData initData;
		initData.pData = bezierControlPoints;
		initData.rowPitch = sizeof(bezierControlPoints);

		mBezierCurveROP.BindVertexStream(0, 
			factory->CreateVertexBuffer(sizeof(bezierControlPoints), EAH_GPU_Read, BufferCreate_Vertex, &initData));

		VertexElement vertexElement[] = { VertexElement(0, VEF_Float2, VEU_Position, 0) };
		mBezierCurveROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));

		mBezierCurveROP.PrimitiveType = PT_Patch_Control_Point_4;
		mBezierCurveROP.SetVertexRange(0, 4);

		mBezierCurveEffect->GetParameterByName("NumSegments")->SetValue(50);
		mBezierCurveEffect->GetParameterByName("NumStrips")->SetValue(1);

		//////////////////////////////////////////////////////////////////////////
		float2 quad[] = { float2(-200, -200),  float2(200, -200),  float2(200, 200),  float2(-200, 200) };
		initData.pData = quad;
		initData.rowPitch = sizeof(quad);

		mTessQuadROP.BindVertexStream(0, 
			factory->CreateVertexBuffer(sizeof(quad), EAH_GPU_Read, BufferCreate_Vertex, &initData));

		mTessQuadROP.VertexDecl = factory->CreateVertexDeclaration(vertexElement, ARRAY_SIZE(vertexElement));

		mTessQuadROP.PrimitiveType = PT_Patch_Control_Point_4;
		mTessQuadROP.SetVertexRange(0, 4);

		mBezierCurveEffect->GetParameterByName("TessLevelOuter")->SetValue(float4(2, 2, 2, 2));
		mBezierCurveEffect->GetParameterByName("TessLevelInner")->SetValue(float2(8, 8));


	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		device->GetScreenFrameBuffer()->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1, 0);

		float w = static_cast<float>( mMainWindow->GetWidth() );
		float h = static_cast<float>( mMainWindow->GetHeight() );
		mBezierCurveEffect->GetParameterByName("ViewportDim")->SetValue(float2(w, h));

		//device->Draw(mBezierCurveEffect->GetTechniqueByName("BezierCurve"), mBezierCurveROP);
		device->Draw(mBezierCurveEffect->GetTechniqueByName("TessQuad"), mTessQuadROP);

		device->GetScreenFrameBuffer()->SwapBuffers();
	}

	void WindowResize(uint32_t width, uint32_t height)
	{
		
	}

protected:

	shared_ptr<Effect> mBezierCurveEffect;
	RenderOperation mBezierCurveROP;
	RenderOperation mTessQuadROP;
};


int main()
{
	TesselationApp app("../Config.xml");
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}