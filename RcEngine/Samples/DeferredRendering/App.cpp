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

#include <GL/GL.h>
#pragma comment(lib, "OpenGL32")
#include "pfm.h"

//shared_ptr<RenderOperation> BuildPointLightShape()
//{
//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
//
//	const int nRings = 30;
//	const int nSegments = 30;
//	const float r = 1.0f;
//
//	VertexElement vdsc[] = {
//		VertexElement(0, VEF_Float3,  VEU_Position, 0),
//		VertexElement(0, VEF_Float2,  VEU_TextureCoordinate, 0),
//	};
//	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 2);
//
//	int32_t vertexCount = (nRings + 1) * (nSegments+1);
//	int32_t indicesCount =  6 * nRings * (nSegments + 1);
//
//	ElementInitData vInitData;
//	vInitData.pData = nullptr;
//	vInitData.rowPitch = 5 * vertexCount * sizeof(float);
//	vInitData.slicePitch = 0;
//	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);
//
//	ElementInitData iInitData;
//	iInitData.pData = nullptr;
//	iInitData.rowPitch = indicesCount * sizeof(unsigned short);
//	iInitData.slicePitch = 0;
//	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);
//
//	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
//	unsigned short* pIndices = static_cast<unsigned short*>(indexBuffer->Map(0, -1, BA_Write_Only));
//
//	float fDeltaRingAngle = Mathf::PI / nRings;
//	float fDeltaSegAngle = Mathf::TWO_PI / nSegments;
//	unsigned short wVerticeIndex = 0 ;
//
//	// Generate the group of rings for the sphere
//	for( int ring = 0; ring <= nRings; ring++ ) {
//		float r0 = r * sinf (ring * fDeltaRingAngle);
//		float y0 = r * cosf (ring * fDeltaRingAngle);
//
//		// Generate the group of segments for the current ring
//		for(int seg = 0; seg <= nSegments; seg++) {
//			float x0 = r0 * sinf(seg * fDeltaSegAngle);
//			float z0 = r0 * cosf(seg * fDeltaSegAngle);
//
//			// Add one vertex to the strip which makes up the sphere
//			*pVertex++ = x0;
//			*pVertex++ = y0;
//			*pVertex++ = z0;
//
//			*pVertex++ = seg / (float)nSegments;
//			*pVertex++ = ring / (float)nRings; 
//
//			if (ring != nRings) {
//				// each vertex (except the last) has six indices pointing to it
//				*pIndices++ = wVerticeIndex + nSegments + 1;
//				*pIndices++ = wVerticeIndex;               
//				*pIndices++ = wVerticeIndex + nSegments;
//				*pIndices++ = wVerticeIndex + nSegments + 1;
//				*pIndices++ = wVerticeIndex + 1;
//				*pIndices++ = wVerticeIndex;
//				wVerticeIndex ++;				
//			}
//		}; // end for seg
//	} // end for ring
//
//	vertexBuffer->UnMap();
//	indexBuffer->UnMap();
//
//	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);
//	mRenderOperation->PrimitiveType = PT_Triangle_List;
//	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
//	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);
//
//	return mRenderOperation;
//}

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

	//shared_ptr<TextureResource> texture = std::static_pointer_cast<TextureResource>(
	//	resMan.GetResourceByName(RT_Texture, "Earth4096.dds", "General"));
	//mMaterial->SetTexture("DiffuseTexure", texture->GetTexture());

	mSphere = BuildPointLightShape();
	mCone = BuildSpotLightShape();

	// Set up lights
	SetupLights();
	
	// Set up camera
	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	mCamera->SetViewParams(float3(85, 68, -21), float3(86, 68, -21));
	mCamera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);

	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);

	mSceneRender->SetRenderPipeline(mDeferredPipeline);
}

void App::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	//Light* sunLight = sceneMan.CreateLight("Sun");
	//sunLight->SetLightType(LT_Directional);
	//sunLight->SetDirection(float3(-1, -1, -1));
	//sunLight->SetLightColor(float3(1, 1, 1));
	//sceneMan.GetRootSceneNode()->AttachObject(sunLight);

	Light* pointLight = sceneMan.CreateLight("Point");
	pointLight->SetLightType(LT_PointLight);
	pointLight->SetLightColor(float3(1, 0, 0));
	pointLight->SetRange(80.0f);
	pointLight->SetAttenuation(1.0f, 0.0f);
	pointLight->SetPosition(float3(550, 81, -18));
	sceneMan.GetRootSceneNode()->AttachObject(pointLight);

	//Light* spotLight = sceneMan.CreateLight("Spot");
	//spotLight->SetLightType(LT_SpotLight);
	//spotLight->SetLightColor(float3(0, 1, 0));
	//spotLight->SetRange(250.0f);
	//spotLight->SetPosition(float3(-442, 70, -16));
	//spotLight->SetDirection(float3(-1, 0, 0));
	//spotLight->SetAttenuation(1.0f, 1.0f);
	//spotLight->SetSpotAngle(Mathf::ToRadian(30), Mathf::ToRadian(40));
	//sceneMan.GetRootSceneNode()->AttachObject(spotLight);
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
	
	//std::vector<float> depth(mMainWindow->GetWidth() * mMainWindow->GetHeight());
	//glReadPixels(0, 0, mMainWindow->GetWidth(), mMainWindow->GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, &depth[0]);
	//WritePfm("E:/depth.pfm", mMainWindow->GetWidth(), mMainWindow->GetHeight(), 1, &depth[0]);

	//float4x4 world = CreateScaling(80.0f, 80.0f, 80.0f) * CreateTranslation(float3(550, 81, -18));
	//mMaterial->ApplyMaterial(world);
	//mMaterial->SetCurrentTechnique("LightShape");
	//device.Render(*mMaterial->GetCurrentTechnique(), *mSphere);

	//float sx, sy, sz;
	//sy = 250.0f;
	//sx = sz = sy * tanf(Mathf::ToRadian(40.0f) * 0.5f);

	//float3 rotAxis = Cross(float3(0, 1, 0), float3(-1, 0, 0));
	//float rotAngle = acosf(Dot(float3(0, 1, 0), float3(-1, 0, 0)));

	//world = CreateScaling(sx, sy, sz) * CreateRotationAxis(rotAxis, rotAngle) *
	//		CreateTranslation(float3(-442, 70, -16.3f));


	//mMaterial->ApplyMaterial(world);
	//mMaterial->SetCurrentTechnique("LightShape");
	//device.Render(*mMaterial->GetCurrentTechnique(), *mCone);

	device.GetScreenFrameBuffer()->SwapBuffers();
}

void App::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);
	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));
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

	const float3& cameraPos = mCamera->GetPosition();
	const float3& cameraLookat = mCamera->GetLookAt();

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"Graphics Demo FPS: %d, Camera Pos (%.1f, %.1f, %.1f), LookAt (%.1f, %.1f, %.1f)", mFramePerSecond,
		cameraPos.X(), cameraPos.Y(), cameraPos.Z(),
		cameraLookat.X(), cameraLookat.Y(), cameraLookat.Z());
	mLabel->SetText(buffer);

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

	mWindow = new UIWindow;
	mWindow->InitGuiStyle(nullptr);
	mWindow->SetName("Panel");
	mWindow->SetTitle(L"Window Title");
	mWindow->SetPosition(int2(20, 50));
	mWindow->SetSize(int2(350, 300));
	rootElem->AddChild( mWindow );	
}

void App::WindowResize(uint32_t width, uint32_t height)
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.SetProjectionParams(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

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