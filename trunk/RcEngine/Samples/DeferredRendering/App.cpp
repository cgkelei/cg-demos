#include "App.h"
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/SamplerState.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <GUI/UIManager.h>
#include <GUI/Label.h>
#include <GUI/UIWindow.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Texture.h>
#include <Graphics/CascadedShadowMap.h>

#include <GL/GL.h>
#pragma comment(lib, "OpenGL32")
#include "pfm.h"

std::vector<float> gSceneDepth;
float3 PickPos;

shared_ptr<RenderOperation> BuildPointLightShape()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	const int nRings = 30;
	const int nSegments = 30;
	const float r = 1.0f;

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	int32_t vertexCount = (nRings + 1) * (nSegments+1);
	int32_t indicesCount =  6 * nRings * (nSegments + 1);

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(unsigned short);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
	unsigned short* pIndices = static_cast<unsigned short*>(indexBuffer->Map(0, -1, BA_Write_Only));

	float fDeltaRingAngle = Mathf::PI / nRings;
	float fDeltaSegAngle = Mathf::TWO_PI / nSegments;
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			if (ring != nRings) {
				// each vertex (except the last) has six indices pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;               
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;				
			}
		}; // end for seg
	} // end for ring

	vertexBuffer->UnMap();
	indexBuffer->UnMap();

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	return mRenderOperation;
}

shared_ptr<RenderOperation> BuildSpotLightShape() 
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	float mRadius = 1.f;
	float mHeight = 1.f;
	uint16_t nCapSegments = 20;

	uint16_t vertexCount = nCapSegments+1;
	uint16_t indicesCount = (nCapSegments+nCapSegments-2)*3;

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(uint16_t);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
	uint16_t* pIndices = static_cast<uint16_t*>(indexBuffer->Map(0, -1, BA_Write_Only));

	std::vector<float3> Vertices;
	std::vector<int> Indices;

	uint16_t topPointOffset = 0;
	*pVertex++ = 0.0f;
	*pVertex++ = 0.0f;
	*pVertex++ = 0.0f;

	int ringStartOffset = 1;
	float deltaAngle = (Mathf::TWO_PI / nCapSegments);
	for (uint16_t i = 0; i < nCapSegments; i++)
	{
		float x0 = mRadius* cosf(i*deltaAngle);
		float z0 = mRadius * sinf(i*deltaAngle);

		*pVertex++ = x0;
		*pVertex++ = mHeight;
		*pVertex++ = z0;
	}

	for (uint16_t i = 0; i < nCapSegments; ++i)
	{
		*pIndices++ = topPointOffset;
		*pIndices++ = ringStartOffset+i;
		*pIndices++ = ringStartOffset+ (i+1)%nCapSegments;
	}

	// Caps
	for (uint16_t i = 0; i < nCapSegments - 2; ++i)
	{
		*pIndices++ = ringStartOffset;
		*pIndices++ = ringStartOffset+i+1+1;
		*pIndices++ = ringStartOffset+i+1;
	}

	vertexBuffer->UnMap();
	indexBuffer->UnMap();

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	return mRenderOperation;
}

App::App( const String& config )
	: Application(config),
	  mSceneRender(nullptr),
	  mFramePerSecond(0)
{

}

App::~App(void)
{
	delete mSceneRender;
}

void App::Initialize()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory =  Context::GetSingleton().GetRenderFactory();

	mSceneRender = new Renderer;
	mSceneRender->Init();

	InitGUI();
}

void App::LoadContent()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	std::cout << "load DeferredLighting.pipeline.xml\n";
	// Load pipeline
	mDeferredPipeline = std::static_pointer_cast<Pipeline>( 
		resMan.GetResourceByName(RT_Pipeline, "DeferredLighting.pipeline.xml", "General"));

	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->SetScale(0.45f);
	sponzaNode->AttachObject(sponzaEntity);

	mMaterial = std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "LightShape.material.xml", "General"));
	mMaterial->Load();

	mSphere = BuildPointLightShape();
	mCone = BuildSpotLightShape();

	// Set up lights
	SetupLights();
	
	// Set up camera
	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	/*mCamera->SetViewParams(float3(85, 68, -21), float3(86, 68, -21));*/
	/*mCamera->SetViewParams(float3(-211, 57, -12), float3(-212, 57, -12));*/
	mCamera->CreateLookAt(float3(-104, 39, -9.8), float3(-54,34.9,-10.8));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCamera->GetFrustum();

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);

	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);

	mSceneRender->SetRenderPipeline(mDeferredPipeline);
}

void App::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	mDirLight = sceneMan.CreateLight("Sun");
	mDirLight->SetLightType(LT_Directional);
	mDirLight->SetDirection(float3(0, -8, -1));
	mDirLight->SetLightColor(float3(1, 1, 1));
	mDirLight->SetCastShadow(false);
	sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

	//mPointLight = sceneMan.CreateLight("Point");
	//mPointLight->SetLightType(LT_PointLight);
	//mPointLight->SetLightColor(float3(1, 1, 0));
	//mPointLight->SetRange(80.0f);
	//mPointLight->SetAttenuation(1.0f, 0.0f);
	//mPointLight->SetPosition(float3(550, 81, -18));
	//sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

	//mSpotLight = sceneMan.CreateLight("Spot");
	//mSpotLight->SetLightType(LT_SpotLight);
	//mSpotLight->SetLightColor(float3(0, 1, 0));
	//mSpotLight->SetRange(250.0f);
	//mSpotLight->SetPosition(float3(-442, 80, -16));
	//mSpotLight->SetDirection(float3(-1, 0, 0));
	//mSpotLight->SetAttenuation(1.0f, 0.0f);
	//mSpotLight->SetSpotAngle(Mathf::ToRadian(30), Mathf::ToRadian(40));
	//sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);


	{
		float3 direction = Normalize(float3(-111.5f, 398.1f, 3.6f) - float3(-111.1, 380.1, 73.4));
		for (int i = 0; i < 4; ++i)
		{
			Light* spotLight = sceneMan.CreateLight("Spot" + std::to_string(i));
			spotLight->SetLightType(LT_SpotLight);
			spotLight->SetLightColor(float3(1, 1, 0));
			spotLight->SetRange(150);
			spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, 3.6f));
			spotLight->SetDirection(direction);
			spotLight->SetAttenuation(1.0f, 0.0f);
			spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
			sceneMan.GetRootSceneNode()->AttachObject(spotLight);
			mSpotLights.push_back(spotLight);
		}

		direction = Normalize(float3(-111.5f, 398.1f, 35.7f) - float3(-111.1, 380.1, -111.3));
		for (int i = 0; i < 4; ++i)
		{
			Light* spotLight = sceneMan.CreateLight("Spot");
			spotLight->SetLightType(LT_SpotLight);
			spotLight->SetLightColor(float3(0, 1, 1));
			spotLight->SetRange(150);
			spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, -35.7f));
			spotLight->SetDirection(direction);
			spotLight->SetAttenuation(1.0f, 0.0f);
			spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
			sceneMan.GetRootSceneNode()->AttachObject(spotLight);
			mSpotLights.push_back(spotLight);
		}
	}

}

void App::UnloadContent()
{
}

void App::Render()
{
	mSceneRender->RenderScene();

	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	device.BindFrameBuffer(device.GetScreenFrameBuffer());
	//device.GetScreenFrameBuffer()->Clear(CF_Depth, ColorRGBA::Black, 1.0f, 0);
	
	//{
	//	float radius = mPointLight->GetRange();
	//	float4x4 worldMatrix = CreateScaling(radius, radius, radius) * CreateTranslation(mPointLight->GetDerivedPosition());

	//	mMaterial->ApplyMaterial(worldMatrix);
	//	mMaterial->SetCurrentTechnique("LightShape");
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mSphere);

	//}
	//
	//{
	//	float scaleHeight = mSpotLight->GetRange();
	//	float scaleBase = scaleHeight * tanf(mSpotLight->GetSpotOuterAngle() * 0.5f);

	//	float3 rotAxis = Cross(float3(0, 1, 0), mSpotLight->GetDerivedDirection());
	//	float rotAngle = acosf(Dot(float3(0, 1, 0), mSpotLight->GetDerivedDirection()));
	//	float4x4 rotation = CreateRotationAxis(rotAxis, rotAngle);

	//	float4x4 world = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
	//		CreateTranslation(mSpotLight->GetDerivedPosition());

	//	mMaterial->ApplyMaterial(world);
	//	mMaterial->SetCurrentTechnique("LightShape");
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mCone);
	//}

	//for (Light* spotLight : mSpotLights)
	//{
	//	float scaleHeight = spotLight->GetRange();
	//	float scaleBase = scaleHeight * tanf(spotLight->GetSpotOuterAngle() * 0.5f);

	//	float3 rotAxis = Cross(float3(0, 1, 0), spotLight->GetDerivedDirection());
	//	float rotAngle = acosf(Dot(float3(0, 1, 0), spotLight->GetDerivedDirection()));
	//	float4x4 rotation = CreateRotationAxis(rotAxis, rotAngle);

	//	float4x4 world = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
	//		CreateTranslation(spotLight->GetDerivedPosition());

	//	mMaterial->ApplyMaterial(world);
	//	mMaterial->SetCurrentTechnique("LightShape");
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mCone);
	//}


	// Target Pos
	{
		float3 lookAt = mCamera->GetPosition() + mCamera->GetView() * 50;
		float radius = 0.2f;
		float4x4 worldMatrix = CreateScaling(radius, radius, radius) * 
			CreateTranslation(lookAt);

		mMaterial->ApplyMaterial(worldMatrix);
		mMaterial->SetCurrentTechnique("LightShape");
		device.Render(*mMaterial->GetCurrentTechnique(), *mSphere);
	}


	//{
	//	float3 orgPos(18, 29, -12);

	//	float scaleHeight = 50;
	//	float scaleBase = scaleHeight * tanf(Mathf::ToRadian(10.0f) * 0.5f);

	//	float3 rotAxis = Cross(float3(0, 1, 0), float3(1, 0, 0));
	//	float rotAngle = acosf(Dot(float3(0, 1, 0), float3(1, 0, 0)));
	//	float4x4 rotation = CreateRotationAxis(rotAxis, rotAngle);

	//	float4x4 world = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
	//		CreateTranslation(orgPos);

	//	// X
	//	mMaterial->SetDiffuseColor(ColorRGBA(1, 0, 0, 1));
	//	mMaterial->ApplyMaterial(world);
	//	mMaterial->SetCurrentTechnique("LightShape");
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mCone);

	//	// Y
	//	world = CreateScaling(scaleBase, scaleHeight, scaleBase) * CreateTranslation(orgPos);
	//	mMaterial->SetDiffuseColor(ColorRGBA(0, 1, 0, 1));
	//	mMaterial->ApplyMaterial(world);
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mCone);

	//	rotAxis = Cross(float3(0, 1, 0), float3(0, 0, 1));
	//	rotAngle = acosf(Dot(float3(0, 1, 0), float3(0, 0, 1)));
	//	rotation = CreateRotationAxis(rotAxis, rotAngle);

	//	// Z
	//	world = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation * CreateTranslation(orgPos);
	//	mMaterial->SetDiffuseColor(ColorRGBA(0, 0, 1, 1));
	//	mMaterial->ApplyMaterial(world);
	//	device.Render(*mMaterial->GetCurrentTechnique(), *mCone);

	//}


	//gSceneDepth.resize(mMainWindow->GetWidth() * mMainWindow->GetHeight());
	//glReadPixels(0, 0, mMainWindow->GetWidth(), mMainWindow->GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, &gSceneDepth[0]);
	//
	
	mSceneRender->DrawOverlays();

	device.GetScreenFrameBuffer()->SwapBuffers();
}

void App::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);
	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));

	float3 target = mCamera->GetPosition() + mCamera->GetView() * 50.0f;

	InputSystem& inputSys = InputSystem::GetSingleton();
	if (inputSys.MouseButtonPress(MS_RightButton))
	{
		uint32_t w = mMainWindow->GetWidth(); 
		uint32_t h = mMainWindow->GetHeight();
		int2 mouse = inputSys.GetMousePos();

		float z = gSceneDepth[(h - mouse.Y() - 1) * w + mouse.X()];

		float x = mouse.X() / float(w) * 2.0f - 1.0f;
		float y = (1.0f - mouse.Y() / float(h)) * 2.0f - 1.0f;

		float4 worldPosH = float4(x, y,z, 1.0) * mCamera->GetInvProjMatrix() * mCamera->GetInvViewMatrix();

		PickPos = float3(worldPosH.X() / worldPosH.W(), worldPosH.Y() / worldPosH.W(), worldPosH.Z() / worldPosH.W()); 
	}

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"FPS: %d, Target Pos (%.1f, %.1f, %.1f), Pick (%.1f, %.1f, %.1f)", mFramePerSecond,
		target.X(), target.Y(), target.Z(),
		PickPos.X(), PickPos.Y(), PickPos.Z());
	mLabel->SetText(buffer);
}

void App::CalculateFrameRate()
{
	static int frameCount = 0;
	static float baseTime = 0;

	frameCount++;

	if (mTimer.GetGameTime()-baseTime >= 1.0f)
	{
		mFramePerSecond = frameCount;
		frameCount = 0;
		baseTime += 1.0f;
	}
}

void App::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mLabel = new Label();
	mLabel->InitGuiStyle(nullptr);
	mLabel->SetName("FPSLabel");
	mLabel->SetPosition(int2(10, 700));
	mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mLabel->SetSize(int2(900, 100));
	//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mLabel );	

	/*mWindow = new UIWindow;
	mWindow->InitGuiStyle(nullptr);
	mWindow->SetName("Panel");
	mWindow->SetTitle(L"Window Title");
	mWindow->SetPosition(int2(20, 50));
	mWindow->SetSize(int2(350, 300));
	rootElem->AddChild( mWindow );	*/
}

void App::WindowResize(uint32_t width, uint32_t height)
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.CreatePerspectiveFov(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

	mDeferredPipeline->Resize(width, height);
}


int main(int argc, char* argv[])
{
	App app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();


	return 0;
}