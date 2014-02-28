#include <Graphics/RenderPath.h>
#include <Graphics/Renderer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderView.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Texture.h>
#include <Graphics/CascadedShadowMap.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Light.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>
#include <GUI/UIManager.h>

namespace {

using namespace RcEngine;

shared_ptr<RenderOperation> BuildFSQuadShape()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	// Fullscreen polygon
	float vertices[6] = {  // x, y, z
		-1.0f, 1.0f, 3.0f, 1.0f, -1.0f, -3.0f, 
	};

	ElementInitData vInitData;
	vInitData.pData = vertices;
	vInitData.rowPitch = 6 * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float2,  VEU_Position, 0),
	};
	shared_ptr<VertexDeclaration> vertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	shared_ptr<RenderOperation> mRenderOperation(new RenderOperation);

	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(vertexBuffer, vertexDecl);
	mRenderOperation->SetVertexRange(0, 3);

	return mRenderOperation;
}

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
	uint16_t nCapSegments = 30;

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

}

namespace RcEngine {

RenderPath::RenderPath()
{
	mDevice = Context::GetSingleton().GetRenderDevicePtr();
	mSceneMan = Context::GetSingleton().GetSceneManagerPtr();
	if (mDevice == NULL || mSceneMan == NULL)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Create device and SceneManager first!", "Renderer::Init");
		return;
	}
}

void RenderPath::OnGraphicsInit()
{
	mFSQuadShape = BuildFSQuadShape();
}

void RenderPath::DrawFSQuad( const shared_ptr<Material>& material, const String& tech )
{
	material->ApplyMaterial();
	material->SetCurrentTechnique(tech);
	mDevice->Render(*material->GetCurrentTechnique(), *mFSQuadShape);
}

void RenderPath::DrawOverlays()
{
	// Update overlays
	UIManager& uiMan = UIManager::GetSingleton();

	uiMan.Render();
	mSceneMan->UpdateOverlayQueue();

	RenderBucket& guiBucket =mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay);	
	for (const RenderQueueItem& renderItem : guiBucket) 
		renderItem.Renderable->Render();
}

//--------------------------------------------------------------------------------------------
DeferredPath::DeferredPath()
	: RenderPath()
{

}

void DeferredPath::OnGraphicsInit()
{
	RenderPath::OnGraphicsInit();

	Window* appWindow = Context::GetSingleton().GetApplication().GetMainWindow();
	const uint32_t windowWidth = appWindow->GetWidth();
	const uint32_t windowHeight = appWindow->GetHeight();

	RenderFactory* factory = mDevice->GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	// Load defered lighting effect
	mDeferedMaterial = std::static_pointer_cast<Material>( 
		resMan.GetResourceByName(RT_Material, "Lighting.material.xml", "General"));
	mDeferedMaterial->Load();

	mDebugViewMaterial = std::static_pointer_cast<Material>( 
		resMan.GetResourceByName(RT_Material, "DebugView.material.xml", "General"));
	mDeferedMaterial->Load();

	// Init GBuffer
	uint16_t acessHint = EAH_GPU_Write | EAH_GPU_Read | EAH_CPU_Read;
	mGBuffer[0] = factory->CreateTexture2D(windowWidth, windowHeight, PF_A8B8G8R8, 1, 1, 0, 0, acessHint, NULL);
	mGBuffer[1] = factory->CreateTexture2D(windowWidth, windowHeight, PF_A8B8G8R8, 1, 1, 0, 0, acessHint, NULL);
	mDepthStencilBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_Depth24Stencil8, 1, 1, 0, 0, acessHint, NULL);

	mGBufferRTV[0] = factory->CreateRenderTargetView2D(mGBuffer[0], 0, 0);
	mGBufferRTV[1] = factory->CreateRenderTargetView2D(mGBuffer[1], 0, 0);
	mDepthStencilView = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);

	mGBufferFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mGBufferFB->Attach(ATT_DepthStencil, mDepthStencilView);
	mGBufferFB->Attach(ATT_Color0, mGBufferRTV[0]);
	mGBufferFB->Attach(ATT_Color1, mGBufferRTV[1]);

	// Init light buffer
	mLightAccumulateBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_A32B32G32R32F, 1, 1, 0, 0, acessHint, NULL);
	mDepthStencilBufferLight = factory->CreateTexture2D(windowWidth, windowHeight, PF_Depth24Stencil8, 1, 1, 0, 0, acessHint, NULL);
	mLightAccumulateRTV = factory->CreateRenderTargetView2D(mLightAccumulateBuffer, 0, 0);
	mDepthStencilBufferLightView = factory->CreateDepthStencilView(mDepthStencilBufferLight, 0, 0);
	
	mLightAccumulateFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mLightAccumulateFB->Attach(ATT_DepthStencil, mDepthStencilBufferLightView);
	mLightAccumulateFB->Attach(ATT_Color0, mLightAccumulateRTV);

	// HDR buffer
	mHDRBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_A32B32G32R32F, 1, 1, 0, 0, acessHint, NULL);
	mHDRBufferRTV = factory->CreateRenderTargetView2D(mHDRBuffer, 0, 0);
	mHDRFB = factory->CreateFrameBuffer(windowWidth, windowHeight);

	mHDRFB->Attach(ATT_DepthStencil, mDepthStencilView);
	mHDRFB->Attach(ATT_Color0, mHDRBufferRTV);


	// Set as view camera
	shared_ptr<Camera> viewCamera = mDevice->GetScreenFrameBuffer()->GetCamera();
	mGBufferFB->SetCamera( viewCamera );
	mLightAccumulateFB->SetCamera(viewCamera);

	// Init shadow manager
	mShadowMan = new CascadedShadowMap(mDevice);
}

void DeferredPath::OnWindowResize( uint32_t width, uint32_t height )
{

}

shared_ptr<Effect> DeferredPath::GetDeferredEffect() const
{
	return mDeferedMaterial->GetEffect();
}

void DeferredPath::RenderScene()
{
	// Stage 0: render GBuffer
	GenereateGBuffer();
	DeferredLighting();
	DeferredShading();
	
	// Draw Sky box




	shared_ptr<FrameBuffer> screenFB = mDevice->GetScreenFrameBuffer();
	mDevice->BindFrameBuffer(screenFB);
	screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0, 0);

	DrawOverlays();

	screenFB->SwapBuffers();
}

void DeferredPath::GenereateGBuffer()
{
	mDevice->BindFrameBuffer(mGBufferFB);
	mGBufferFB->Clear(CF_Color | CF_Depth | CF_Stencil, ColorRGBA(0, 0, 0, 0), 1.0, 0);

	const String techName = "GBuffer";

	// Todo: update render queue with render bucket filter
	const Camera& camera = *(mGBufferFB->GetCamera());	
	mSceneMan->UpdateRenderQueue(camera, RO_None);   

	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	
	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		renderItem.Renderable->GetMaterial()->SetCurrentTechnique(techName);
		renderItem.Renderable->Render();
	}

	//mDevice->GetRenderFactory()->SaveTexture2D("E:/GBuffer0.tga", mGBuffer[0], 0, 0);
	//mDevice->GetRenderFactory()->SaveTexture2D("E:/GBuffer1.tga", mGBuffer[1], 0, 0);
}

void DeferredPath::ComputeSSAO()
{

}

void DeferredPath::DeferredLighting()
{
	const std::vector<Light*>& sceneLights = mSceneMan->GetSceneLights();

	mDevice->BindFrameBuffer(mLightAccumulateFB);
	mLightAccumulateRTV->ClearColor(ColorRGBA(0, 0, 0, 0));

	// Copy depth and stencil
	mDepthStencilBuffer->CopyToTexture(*mDepthStencilBufferLight);

	mDeferedMaterial->SetTexture("GBuffer0", mGBuffer[0]);
	mDeferedMaterial->SetTexture("DepthBuffer", mDepthStencilBuffer);
	
	const String& tech = "Lighting";

 	bool stencilZFail = false;
	for (size_t i = 0; i < sceneLights.size(); ++i)
	{
		Light* light = sceneLights[i];
		LightType lightType = light->GetLightType();
	
		if (lightType == LT_Directional)
		{
			DrawDirectionalLightShape(light, tech);
		}
		else 
		{
			if (!stencilZFail)
			{
				mDepthStencilBufferLightView->ClearStencil(0);
				stencilZFail = true;
			}		
		}
	}
}

void DeferredPath::DrawDirectionalLightShape( Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());

	bool bCastShadow = light->GetCastShadow();

	if (bCastShadow)
	{
		// Generate shadow map
		mShadowMan->MakeCascadedShadowMap(*light);
	}

	shared_ptr<Effect> effect = mDeferedMaterial->GetEffect();

	const float3& lightColor = light->GetLightColor();
	mDeferedMaterial->GetCustomParameter(EPU_Light_Color)->SetValue(lightColor);

	const float3& worldDirection = light->GetDerivedDirection();
	float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
	lightDir = lightDir * currCamera.GetViewMatrix();
	mDeferedMaterial->GetCustomParameter(EPU_Light_Dir)->SetValue(lightDir);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	effect->GetParameterByName("CameraNearFar")->SetValue(camNearFar);
	effect->GetParameterByName("ShadowEnabled")->SetValue(bCastShadow);
	if (bCastShadow)
	{	
		mDeferedMaterial->SetTexture("ShadowTex", mShadowMan->mShadowTexture);
		effect->GetParameterByName("ShadowView")->SetValue(mShadowMan->mShadowView);
		effect->GetParameterByName("NumCascades")->SetValue((int)light->GetShadowCascades());
		effect->GetParameterByName("BorderPaddingMinMax")->SetValue(mShadowMan->mBorderPaddingMinMax);
		effect->GetParameterByName("CascadeScale")->SetValue(mShadowMan->mShadowCascadeScale);
		effect->GetParameterByName("CascadeOffset")->SetValue(mShadowMan->mShadowCascadeOffset);
		//effect->GetParameterByName("CascadeBlendArea")->SetValue(mCascadedShadowMap->mCascadeBlendArea);
	}

	String techName = "Directional" + tech;
	DrawFSQuad(mDeferedMaterial, techName);
}

void DeferredPath::DeferredShading()
{
	mDevice->BindFrameBuffer(mHDRFB);
	mHDRBufferRTV->ClearColor(ColorRGBA(0, 0, 0, 0));

	mDeferedMaterial->SetTexture("GBuffer0", mGBuffer[0]);
	mDeferedMaterial->SetTexture("GBuffer1", mGBuffer[1]);
	mDeferedMaterial->SetTexture("LightAccumulateBuffer", mLightAccumulateBuffer);

	DrawFSQuad(mDeferedMaterial, "Shading");

	/*mDevice->GetRenderFactory()->SaveTexture2D("E:/HDR.pfm", mHDRBuffer, 0, 0);*/
}



}
