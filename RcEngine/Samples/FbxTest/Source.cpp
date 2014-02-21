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
#include <GUI/UIManager.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <Graphics/CascadedShadowMap.h>
#include <Scene/Light.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>

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


class SponzaApp : public Application
{
public:
	SponzaApp(const String& config)
		:Application(config), mCameraControler(0), mFramePerSecond(0)
	{
	}
	~SponzaApp(void)
	{

	}
protected:

	void Initialize() 
	{
		mSceneRender = new Renderer;
		mSceneRender->Init();

		mSphere = BuildPointLightShape();

		InitGUI();
	}

	void InitGUI()
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
	}

	void LoadContent()
	{
		RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
		SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		RenderDevice& device = Context::GetSingleton().GetRenderDevice();

		FileSystem::GetSingleton().RegisterPath("../Media/Mesh/Tree", "Custom");

		std::cout << "load Fbx.pipeline.xml\n";
		mPipeline = std::static_pointer_cast<Pipeline>( 
			resMan.GetResourceByName(RT_Pipeline, "DeferredLighting.pipeline.xml", "General"));

		mCamera = device.GetScreenFrameBuffer()->GetCamera();
		/*mCamera->CreateLookAt(float3(588.8, 635.6, -1888.5), float3(570.4, 623, -1844.9));*/
		//mCamera->CreateLookAt(float3(2583.9, 1182.3, -292.5), float3(2583.4, 1181.8, -293.0), float3(-0.4, 0.8, -0.4));
		mCamera->CreateLookAt(float3(-331.9, 1094.3, 1756.1), float3(-331.8, 1093.7, 1755.4), float3(0.1, 0.8, -0.6));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 8000.0f );

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);

		std::cout << std::endl << std::endl;
		// Load Scene
		Entity* sponzaEntity = sceneMan.CreateEntity("Tree", "Tree.mesh",  "Custom");
		SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Tree");
		sponzaNode->SetPosition(float3(0, 0, 0));
		sponzaNode->AttachObject(sponzaEntity);

		mCameraControler->SetMoveSpeed(400.0f);
		mCameraControler->SetMoveInertia(true);

		mSceneRender->SetRenderPipeline(mPipeline);

		mDirLight = sceneMan.CreateLight("Sun");
		mDirLight->SetLightType(LT_Directional);
		mDirLight->SetDirection(float3(0, -1, -1));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetCastShadow(true);
		mDirLight->SetShadowCascades(4);
		sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

		mMaterial = std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "LightShape.material.xml", "General"));
		mMaterial->Load();
	}

	void UnloadContent() {}

	void Update(float deltaTime) 
	{

		mCameraControler->Update(deltaTime);

		float3 camera = mCamera->GetPosition();
		float3 target = mCamera->GetLookAt();
		float3 up = mCamera->GetUp();

		wchar_t buffer[255];
		int cx = swprintf (buffer, 255, L"FPS: %d, Camera(%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f)", mFramePerSecond,
			camera.X(), camera.Y(), camera.Z(), target.X(), target.Y(), target.Z(),
			up.X(), up.Y(), up.Z());
		mLabel->SetText(buffer);


		if (InputSystem::GetSingleton().KeyPress(KC_Space))
		{
			EffectParameter* effectParam = mPipeline->GetMaterial(0)->GetEffect()->GetParameterByName("VisiualizeCascades");
			bool enable;
			effectParam->GetValue(enable);
			effectParam->SetValue(!enable);
		}
	}

	void Render()
	{
		mSceneRender->RenderScene();
		
		RenderDevice& device = Context::GetSingleton().GetRenderDevice();

		device.BindFrameBuffer(device.GetScreenFrameBuffer());

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

		mSceneRender->DrawOverlays();
		
		device.GetScreenFrameBuffer()->SwapBuffers();
	}

	void CalculateFrameRate()
	{
	}

protected:
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;
	shared_ptr<Pipeline> mPipeline;
	Renderer* mSceneRender;

	shared_ptr<RenderOperation> mSphere;
	shared_ptr<Material> mMaterial;
	Label* mLabel;
	Light* mDirLight;
	CascadedShadowMap* mCascadedShadowMap;
};


int main(int argc, char* argv[])
{
	SponzaApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}