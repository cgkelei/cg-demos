#include "App.h"
#include <MainApp/Application.h>
#include <Graphics/CameraController1.h>
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
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
#include <Graphics/Pipeline.h>
#include <Graphics/Renderer.h>
#include <Graphics/FrameBuffer.h>
#include <GUI/Label.h>
#include <GUI/Slider.h>
#include <GUI/UIWindow.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <GUI/CheckBox.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <Graphics/CascadedShadowMap.h>
#include <Scene/Light.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Texture.h>

using namespace RcEngine;

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

RenderPathApp::RenderPathApp( const String& config ) 
	:Application(config), mCameraControler(0), mFramePerSecond(0)
{

}

RenderPathApp::~RenderPathApp( void )
{

}

void RenderPathApp::Initialize()
{
	mDeferredPath = new DeferredPath();
	mDeferredPath->OnGraphicsInit();

	mSphere = BuildPointLightShape();
	mCone = BuildSpotLightShape();

	InitGUI();
}

void RenderPathApp::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mLabel = new Label();
	mLabel->InitGuiStyle(nullptr);
	mLabel->SetName("FPSLabel");
	mLabel->SetPosition(int2(10, 700));
	mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mLabel->SetSize(int2(100, 100));
	//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mLabel );	

	mWindow = new UIWindow;
	mWindow->InitGuiStyle(nullptr);
	mWindow->SetName("Panel");
	mWindow->SetTitle(L"Window Title");
	mWindow->SetPosition(int2(650, 430));
	mWindow->SetSize(int2(350, 300));
	rootElem->AddChild( mWindow );

	int uiY = 50;

	mSunShadowCheckBox = new CheckBox();
	mSunShadowCheckBox->InitGuiStyle(nullptr);
	mSunShadowCheckBox->SetName("Sun Shadow");
	mSunShadowCheckBox->SetPosition(int2(20, uiY));
	mSunShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	mSunShadowCheckBox->SetText(L"Sun Shadow");
	mSunShadowCheckBox->SetChecked(true);
	mSunShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SunCastShadow);
	mWindow->AddChild( mSunShadowCheckBox );	

	mSpotShadowCheckBox = new CheckBox();
	mSpotShadowCheckBox->InitGuiStyle(nullptr);
	mSpotShadowCheckBox->SetName("Spot Shadow");
	mSpotShadowCheckBox->SetPosition(int2(20 + mSunShadowCheckBox->GetSize().X() + 20, uiY));
	mSpotShadowCheckBox->SetSize(int2(150, mSunShadowCheckBox->GetSize().Y()));
	mSpotShadowCheckBox->SetText(L"Spot Shadow");
	mSpotShadowCheckBox->SetChecked(true);
	mSpotShadowCheckBox->EventStateChanged.bind(this, &RenderPathApp::SpotCastShadow);
	mWindow->AddChild( mSpotShadowCheckBox );	

	
	uiY += mSunShadowCheckBox->GetSize().Y() + 18;

	mVisualizeCascadesCheckBox = new CheckBox();
	mVisualizeCascadesCheckBox->InitGuiStyle(nullptr);
	mVisualizeCascadesCheckBox->SetName("CheckBox");
	mVisualizeCascadesCheckBox->SetPosition(int2(20, uiY));
	mVisualizeCascadesCheckBox->SetSize(int2(150, mVisualizeCascadesCheckBox->GetSize().Y()));
	mVisualizeCascadesCheckBox->SetText(L"Visualize Cascades");
	mVisualizeCascadesCheckBox->SetChecked(false);
	mVisualizeCascadesCheckBox->EventStateChanged.bind(this, &RenderPathApp::VisualizeCascades);
	mWindow->AddChild( mVisualizeCascadesCheckBox );	

	uiY += mVisualizeCascadesCheckBox->GetSize().Y() + 18;

	mBlendAreaLabel = new Label();
	mBlendAreaLabel->InitGuiStyle(nullptr);
	mBlendAreaLabel->SetName("BlendArea");
	mBlendAreaLabel->SetPosition(int2(20, uiY));
	mBlendAreaLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mBlendAreaLabel->SetTextAlignment(AlignLeft);
	mBlendAreaLabel->SetSize(int2(150, 30));
	mBlendAreaLabel->SetText(L"BlendArea: 0.01");
	mBlendAreaLabel->SetFontSize(20.0f);
	//mBlendAreaLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	mWindow->AddChild( mBlendAreaLabel );	

	mBlendAreaSlider = new Slider(UI_Horizontal);
	mBlendAreaSlider->InitGuiStyle(nullptr);
	mBlendAreaSlider->SetName("Slider");	
	mBlendAreaSlider->SetPosition(int2(20 + mBlendAreaLabel->GetSize().X(), uiY + 5));
	mBlendAreaSlider->SetTrackLength(120);
	mBlendAreaSlider->SetValue(50);
	mBlendAreaSlider->EventValueChanged.bind(this, &RenderPathApp::BlendAreaSliderValueChange);
	mWindow->AddChild( mBlendAreaSlider );	

	uiY += mBlendAreaLabel->GetSize().Y();

	mSaveCameraBtn = new Button();
	mSaveCameraBtn->InitGuiStyle(nullptr);
	mSaveCameraBtn->SetName("Button");	
	mSaveCameraBtn->SetPosition(int2(20, uiY));
	mSaveCameraBtn->SetSize(int2(120, 30));
	mSaveCameraBtn->SetText(L"Dump Camera");
	mSaveCameraBtn->EventButtonClicked.bind(this, &RenderPathApp::DumpCameraBtnClicked);
	mWindow->AddChild( mSaveCameraBtn );	
}

void RenderPathApp::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	FileSystem::GetSingleton().RegisterPath("../Media/Mesh/Tree", "Custom");

	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	
	mCamera->CreateLookAt(float3(408.8, 1053.9, 279.3), float3(408.6, 1053.1, 278.8), float3(-0.4, 0.6, -0.7));
	//mCamera->CreateLookAt(float3(-395.7, 839.9, 2061.9), float3(-395.4, 839.6, 2061.0), float3(0.1, 0.9, -0.4));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 8000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);

	std::cout << std::endl << std::endl;
	
	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Tree", "Tree.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Tree");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->AttachObject(sponzaEntity);
	
	// Load Skybox
	shared_ptr<TextureResource> skyBox = resMan.GetResourceByName<TextureResource>(RT_Texture, "MeadowTrail.dds", "General");
	sceneMan.CreateSkyBox(skyBox->GetTexture());


	mCameraControler->SetMoveSpeed(400.0f);
	mCameraControler->SetMoveInertia(true);


	mDirLight = sceneMan.CreateLight("Sun", LT_Directional);
	mDirLight->SetDirection(float3(0, -1, -1));
	mDirLight->SetLightColor(float3(1, 1, 1));
	mDirLight->SetCastShadow(true);
	mDirLight->SetShadowCascades(4);
	sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

	mSpotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
	mSpotLight->SetDirection(float3(0, -1.5, -1));
	mSpotLight->SetLightColor(float3(1, 1, 0));
	mSpotLight->SetRange(1500.0);
	mSpotLight->SetPosition(float3(0.0f, 800.0f, 0.0f));
	mSpotLight->SetAttenuation(1.0f, 0.0f);
	mSpotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
	mSpotLight->SetCastShadow(true);
	mSpotLight->SetSpotlightNearClip(100);
	sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);

	mMaterial = std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "LightShape.material.xml", "General"));
	mMaterial->Load();

	shared_ptr<Effect> shadowEffect = mDeferredPath->GetDeferredEffect();
	shadowEffect->GetParameterByName("ShadowEnabled")->SetValue(true);
	shadowEffect->GetParameterByName("VisiualizeCascades")->SetValue(false);
}

void RenderPathApp::Update( float deltaTime )
{
	CalculateFrameRate();

	mCameraControler->Update(deltaTime);

	float3 camera = mCamera->GetPosition();
	float3 target = mCamera->GetLookAt();
	float3 up = mCamera->GetUp();

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"FPS: %d", mFramePerSecond);
	mLabel->SetText(buffer);
}

void RenderPathApp::Render()
{
	mDeferredPath->RenderScene();
}

void RenderPathApp::CalculateFrameRate()
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

void RenderPathApp::VisualizeCascades( bool checked )
{
	EffectParameter* effectParam = mDeferredPath->GetDeferredEffect()->GetParameterByName("VisiualizeCascades");
	bool enable;
	effectParam->GetValue(enable);
	effectParam->SetValue(!enable);
}

void RenderPathApp::SunCastShadow( bool checked )
{
	bool enable = mDirLight->GetCastShadow();
	mDirLight->SetCastShadow(!enable);
}

void RenderPathApp::SpotCastShadow( bool checked )
{
	bool enable = mSpotLight->GetCastShadow();
	mSpotLight->SetCastShadow(!enable);
}

void RenderPathApp::BlendAreaSliderValueChange( int32_t value )
{
	float area = value / float(100.0) * 0.5f;
	mDeferredPath->GetShadowManager()->mCascadeBlendArea = area;
	
	wchar_t text[255];
	std::swprintf(text, 255, L"BlendArea: %.3f", area);
	mBlendAreaLabel->SetText(text);
}

void RenderPathApp::DumpCameraBtnClicked()
{
	float3 camera = mCamera->GetPosition();
	float3 target = mCamera->GetLookAt();
	float3 up = mCamera->GetUp();
	
	FILE * pFile;
	pFile = fopen ("E:/camera.txt","w");
	if (pFile!=NULL)
	{
		fprintf (pFile, "float3(%.1f, %.1f, %.1f), float3(%.1f, %.1f, %.1f), float3(%.1f, %.1f, %.1f)",
			camera.X(), camera.Y(), camera.Z(), target.X(), target.Y(), target.Z(),
			up.X(), up.Y(), up.Z());
		fclose (pFile);
	}
}

int main(int argc, char* argv[])
{
	//wchar_t text1[255];
	//std::swprintf(text1, 255, L"BlendArea: %f", 1.1111);
	//wsprintf(text1, L"BlendArea: %.3f", 1.1111);
	//std::wcout << L"Test " << text1;
	
	RenderPathApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}