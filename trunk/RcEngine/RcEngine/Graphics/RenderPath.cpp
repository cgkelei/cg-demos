#include <Graphics/RenderPath.h>
#include <Graphics/Renderer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/CascadedShadowMap.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Light.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

#include <Input/InputSystem.h>

namespace {

using namespace RcEngine;

shared_ptr<RenderOperation> BuildPointLightShape()
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	const int nRings = 30;
	const int nSegments = 30;
	const float r = 1.0f;

	int32_t vertexCount = (nRings + 1) * (nSegments+1);
	int32_t indicesCount =  6 * nRings * (nSegments + 1);

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory->CreateVertexBuffer(vInitData.rowPitch, EAH_GPU_Read | EAH_GPU_Write, BufferCreate_Vertex, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(unsigned short);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory->CreateIndexBuffer(iInitData.rowPitch, EAH_GPU_Read | EAH_GPU_Write, BufferCreate_Index, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
	unsigned short* pIndices = static_cast<unsigned short*>(indexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));

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
	mRenderOperation->BindVertexStream(0, vertexBuffer);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	mRenderOperation->VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);

	return mRenderOperation;
}

shared_ptr<RenderOperation> BuildSpotLightShape() 
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	float mRadius = 1.f;
	float mHeight = 1.f;
	uint16_t nCapSegments = 30;

	uint16_t vertexCount = nCapSegments+1;
	uint16_t indicesCount = (nCapSegments+nCapSegments-2)*3;

	ElementInitData vInitData;
	vInitData.pData = nullptr;
	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
	vInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> vertexBuffer= factory->CreateVertexBuffer(vInitData.rowPitch, EAH_GPU_Read | EAH_GPU_Write, BufferCreate_Index, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = nullptr;
	iInitData.rowPitch = indicesCount * sizeof(uint16_t);
	iInitData.slicePitch = 0;
	shared_ptr<GraphicsBuffer> indexBuffer = factory->CreateIndexBuffer(iInitData.rowPitch, EAH_GPU_Read | EAH_GPU_Write, BufferCreate_Index, &iInitData);

	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));
	uint16_t* pIndices = static_cast<uint16_t*>(indexBuffer->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard));

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
	mRenderOperation->BindVertexStream(0, vertexBuffer);
	mRenderOperation->BindIndexStream(indexBuffer, IBT_Bit16);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	mRenderOperation->VertexDecl = factory->CreateVertexDeclaration(vdsc, 1);


	return mRenderOperation;
}

}

namespace RcEngine {

RenderPath::RenderPath()
{
	mDevice = Environment::GetSingleton().GetRenderDevice();
	mSceneMan = Environment::GetSingleton().GetSceneManager();
	if (mDevice == NULL || mSceneMan == NULL)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Create device and SceneManager first!", "Renderer::Init");
		return;
	}
}

void RenderPath::OnGraphicsInit()
{
	//mFSQuadShape = BuildFSQuadShape();
}

void RenderPath::DrawFSQuad( const shared_ptr<Material>& material, const String& tech )
{
	material->ApplyMaterial();
	material->SetCurrentTechnique(tech);
	mDevice->Draw(material->GetCurrentTechnique(), *mFSQuadShape);
}

void RenderPath::DrawOverlays()
{
	//// Update overlays
	//UIManager& uiMan = UIManager::GetSingleton();

	//uiMan.Render();
	//mSceneMan->UpdateOverlayQueue();

	//RenderBucket& guiBucket =mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOverlay);	
	//for (const RenderQueueItem& renderItem : guiBucket) 
	//	renderItem.Renderable->Render();
}

//----------------------------------------------------------------------------------------------
ForwardPath::ForwardPath()
	: RenderPath()
{

}

void ForwardPath::OnGraphicsInit()
{

}

void ForwardPath::OnWindowResize( uint32_t width, uint32_t height )
{

}

void ForwardPath::RenderScene()
{
	shared_ptr<FrameBuffer> screenFB = mDevice->GetScreenFrameBuffer();
	mDevice->BindFrameBuffer(screenFB);
	screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::White, 1.0, 0);

	auto viewCamera = screenFB->GetCamera();

	auto v = Normalize(Transform(float3(0, 0, 0), viewCamera->GetViewMatrix()));

	// Draw Sky box first
	mSceneMan->UpdateBackgroundQueue(*viewCamera);
	RenderBucket& bkgBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketBackground, false);
	for (RenderQueueItem& item : bkgBucket)
		item.Renderable->Render();

	mSceneMan->UpdateLightQueue(*viewCamera);
	const LightQueue& sceneLights = mSceneMan->GetLightQueue();

	// Draw opaque 
	mSceneMan->UpdateRenderQueue(*viewCamera, RO_None);   

	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);

	for (Light* light : sceneLights)
	{
		String techName;
		if (light->GetLightType() == LT_DirectionalLight)
		{
			techName = "DirectionalLighting";

			for (const RenderQueueItem& renderItem : opaqueBucket) 
			{
				auto material = renderItem.Renderable->GetMaterial();

				material->SetCurrentTechnique(techName);
				
				float3 lightColor = light->GetLightColor() * light->GetLightIntensity();
				material->GetEffect()->GetParameterByUsage(EPU_Light_Color)->SetValue(lightColor);

				const float3& worldDirection = light->GetDerivedDirection();
				float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
				lightDir = lightDir * viewCamera->GetViewMatrix();
				material->GetEffect()->GetParameterByUsage(EPU_Light_Dir)->SetValue(lightDir);

				renderItem.Renderable->Render();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
DeferredPath::DeferredPath()
	: RenderPath(),
	  mVisualLights(false),
	  mVisualLightsWireframe(false)
{

}

void DeferredPath::OnGraphicsInit()
{
	RenderPath::OnGraphicsInit();
		
	// Build light volume
	mSpotLightShape = BuildSpotLightShape();
	mPointLightShape = BuildPointLightShape();

	Window* appWindow = Application::msApp->GetMainWindow();
	const uint32_t windowWidth = appWindow->GetWidth();
	const uint32_t windowHeight = appWindow->GetHeight();

	RenderFactory* factory = mDevice->GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	// Load deferred lighting effect
	mDeferredEffect = resMan.GetResourceByName<Effect>(RT_Effect, "DeferredLighting.effect.xml", "General");

	/*mDeferedMaterial = std::static_pointer_cast<Material>( 
		resMan.GetResourceByName(RT_Material, "Lighting.material.xml", "General"));
	mDeferedMaterial->Load();*/

	//mDebugViewMaterial = std::static_pointer_cast<Material>( 
	//	resMan.GetResourceByName(RT_Material, "DebugView.material.xml", "General"));
	//mDeferedMaterial->Load();

	//mDebugLightMaterial = std::static_pointer_cast<Material>( 
	//	resMan.GetResourceByName(RT_Material, "LightShape.material.xml", "General"));
	//mDebugLightMaterial->Load();

	// Init GBuffer
	mGBufferFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mGBufferFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

#ifdef _DEBUG
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read | EAH_CPU_Read;
#else
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read;
#endif
	uint32_t createFlags = TexCreate_ShaderResource | TexCreate_RenderTarget;

	mGBuffer[0] = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA8_UNORM, 1, 1, 1, 0, acessHint, createFlags, NULL);
	mGBuffer[1] = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA8_UNORM, 1, 1, 1, 0, acessHint, createFlags, NULL);
	mDepthStencilBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_D24S8, 1, 1, 1, 0, acessHint, createFlags, NULL);

	mGBufferRTV[0] = factory->CreateRenderTargetView2D(mGBuffer[0], 0, 0);
	mGBufferRTV[1] = factory->CreateRenderTargetView2D(mGBuffer[1], 0, 0);
	mDepthStencilView = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);

	mGBufferFB->AttachRTV(ATT_DepthStencil, mDepthStencilView);
	mGBufferFB->AttachRTV(ATT_Color0, mGBufferRTV[0]);
	mGBufferFB->AttachRTV(ATT_Color1, mGBufferRTV[1]);

	// Init light buffer
	mLightAccumulateFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mLightAccumulateFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

	mLightAccumulateBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA32F, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mDepthStencilBufferLight = factory->CreateTexture2D(windowWidth, windowHeight, PF_D24S8, 1, 1, 0, 0, acessHint, createFlags, NULL);
	
	mLightAccumulateRTV = factory->CreateRenderTargetView2D(mLightAccumulateBuffer, 0, 0);
	mDepthStencilBufferLightView = factory->CreateDepthStencilView(mDepthStencilBufferLight, 0, 0);
	
	mLightAccumulateFB->AttachRTV(ATT_DepthStencil, mDepthStencilBufferLightView);
	mLightAccumulateFB->AttachRTV(ATT_Color0, mLightAccumulateRTV);

	// HDR buffer
	mHDRFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mHDRFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

	mHDRBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA32F, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mHDRBufferRTV = factory->CreateRenderTargetView2D(mHDRBuffer, 0, 0);
	mHDRFB->AttachRTV(ATT_Color0, mHDRBufferRTV);

	mDepthStencilViewReadOnly = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);
	mHDRFB->AttachRTV(ATT_DepthStencil, mDepthStencilViewReadOnly);

	// Set as view camera
	shared_ptr<Camera> viewCamera = mDevice->GetScreenFrameBuffer()->GetCamera();
	mGBufferFB->SetCamera( viewCamera );
	mLightAccumulateFB->SetCamera(viewCamera);
	mHDRFB->SetCamera(viewCamera);

	// Init shadow manager
	mShadowMan = new CascadedShadowMap(mDevice);
}

void DeferredPath::OnWindowResize( uint32_t windowWidth, uint32_t windowHeight )
{
	RenderFactory* factory = mDevice->GetRenderFactory();

	mDevice->BindFrameBuffer(mGBufferFB);
	mGBufferFB->DetachAll();
	mGBufferFB->Resize(windowWidth, windowHeight);
	mGBufferFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

#ifdef _DEBUG
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read | EAH_CPU_Read;
#else
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read;
#endif
	uint32_t createFlags = TexCreate_ShaderResource | TexCreate_RenderTarget;

	mGBuffer[0] = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA8_UNORM, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mGBuffer[1] = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA8_UNORM, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mDepthStencilBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_D24S8, 1, 1, 0, 0, acessHint, createFlags, NULL);

	mGBufferRTV[0] = factory->CreateRenderTargetView2D(mGBuffer[0], 0, 0);
	mGBufferRTV[1] = factory->CreateRenderTargetView2D(mGBuffer[1], 0, 0);
	mDepthStencilView = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);

	mGBufferFB->AttachRTV(ATT_DepthStencil, mDepthStencilView);
	mGBufferFB->AttachRTV(ATT_Color0, mGBufferRTV[0]);
	mGBufferFB->AttachRTV(ATT_Color1, mGBufferRTV[1]);

	mDevice->BindFrameBuffer(mLightAccumulateFB);
	mLightAccumulateFB->DetachAll();
	mLightAccumulateFB->Resize(windowWidth, windowHeight);
	mLightAccumulateFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

	mLightAccumulateBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA32F, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mDepthStencilBufferLight = factory->CreateTexture2D(windowWidth, windowHeight, PF_D24S8, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mLightAccumulateRTV = factory->CreateRenderTargetView2D(mLightAccumulateBuffer, 0, 0);
	mDepthStencilBufferLightView = factory->CreateDepthStencilView(mDepthStencilBufferLight, 0, 0);

	mLightAccumulateFB->AttachRTV(ATT_DepthStencil, mDepthStencilBufferLightView);
	mLightAccumulateFB->AttachRTV(ATT_Color0, mLightAccumulateRTV);

	mDevice->BindFrameBuffer(mHDRFB);
	mHDRFB->DetachAll();
	mHDRFB->Resize(windowWidth, windowHeight);
	mHDRFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));

	// HDR buffer
	mHDRBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA32F, 1, 1, 0, 0, acessHint, createFlags, NULL);
	mHDRBufferRTV = factory->CreateRenderTargetView2D(mHDRBuffer, 0, 0);
	mHDRFB = factory->CreateFrameBuffer(windowWidth, windowHeight);

	mDepthStencilViewReadOnly = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);
	mHDRFB->AttachRTV(ATT_DepthStencil, mDepthStencilViewReadOnly);
	mHDRFB->AttachRTV(ATT_Color0, mHDRBufferRTV);
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
	
	//shared_ptr<FrameBuffer> screenFB = mDevice->GetScreenFrameBuffer();
	//mDevice->BindFrameBuffer(screenFB);
	//screenFB->Clear(CF_Color | CF_Depth, ColorRGBA::Black, 1.0, 0);




	//mDebugViewMaterial->SetTexture("InputTex", mHDRBuffer->GetShaderResourceView());

	////DebugViewMaterial->SetTexture("InputTex", mLightAccumulateBuffer);
	//DrawFSQuad(mDebugViewMaterial, "ViewRGB");
	//DrawOverlays();

	/*mDeferedMaterial->SetTexture("DepthBuffer", mDepthStencilBuffer);
	DrawFSQuad(mDeferedMaterial, "CopyDepth");*/

	// Copy depth
	//mVisualLights =  mVisualLightsWireframe = true;
	//if (mVisualLights)
	//{
	//	mDeferredEffect->GetParameterByName("DepthBuffer")

	//	mDeferedMaterial->SetTexture("DepthBuffer", mDepthStencilBuffer->GetShaderResourceView());
	//	DrawFSQuad(mDeferedMaterial, "CopyDepth");

	//	String techName = "LightShape";
	//	if (mVisualLightsWireframe)
	//		techName = "LightShapeWireFrame";

	//	const std::vector<Light*>& sceneLights = mSceneMan->GetSceneLights();
	//	for (Light* light : sceneLights)
	//	{
	//		if (light->GetLightType() == LT_PointLight)
	//		{
	//			auto lightColor = light->GetLightColor();
	//			mDebugLightMaterial->SetDiffuseColor(lightColor);
	//			
	//			float radius = light->GetRange(); 
	//			radius = (std::min)(10.0f, radius * 0.1f);
	//			float4x4 world = CreateScaling(radius, radius, radius) * CreateTranslation(light->GetDerivedPosition());
	//			mDebugLightMaterial->ApplyMaterial(world);

	//			mDebugLightMaterial->SetCurrentTechnique(techName);
	//			mDevice->Render(*mDebugLightMaterial->GetCurrentTechnique(), *mPointLightShape);
	//		}
	//		else if (light->GetLightType() == LT_SpotLight) 
	//		{
	//			auto lightColor = light->GetLightColor();
	//			mDebugLightMaterial->SetDiffuseColor(lightColor);

	//			float spotInnerAngle = light->GetSpotInnerAngle();
	//			float spotOuterAngle = light->GetSpotOuterAngle();

	//			float scaleHeight = light->GetRange();
	//			float scaleBase = scaleHeight * tanf(spotOuterAngle * 0.5f);

	//			const float3& worldPos = light->GetDerivedPosition();
	//			const float3& worldDir = light->GetDerivedDirection();
	//			float3 rotAxis = Cross(float3(0, 1, 0), worldDir);
	//			//float rotAngle = acosf(Dot(float3(0, 1, 0), worldDirection));
	//			float4x4 rotation = CreateRotationAxis(rotAxis, acosf(worldDir.Y()));

	//			float4x4 world = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
	//									CreateTranslation(worldPos);
	//			mDebugLightMaterial->ApplyMaterial(world);

	//			mDebugLightMaterial->SetCurrentTechnique(techName);
	//			mDevice->Draw(mDebugLightMaterial->GetCurrentTechnique(), *mSpotLightShape);
	//		}
	//	}
	//}
	//
	//screenFB->SwapBuffers();
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

	if ( InputSystem::GetSingleton().MouseButtonPress(MS_MiddleButton) )
	{
		//mDevice->GetRenderFactory()->SaveTexture2D("E:/GBuffer0.tga", mGBuffer[0], 0, 0);
		//mDevice->GetRenderFactory()->SaveTexture2D("E:/GBuffer1.tga", mGBuffer[1], 0, 0);
	}
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

	EffectParameter* effectParam;

	effectParam = mDeferredEffect->GetParameterByName("GBuffer0");
	effectParam->SetValue(mGBuffer[0]->GetShaderResourceView());

	effectParam = mDeferredEffect->GetParameterByName("DepthBuffer");
	effectParam->SetValue(mDepthStencilBuffer->GetShaderResourceView());

	const String& tech = "Lighting";

 	bool stencilZFail = false;
	for (size_t i = 0; i < sceneLights.size(); ++i)
	{
		Light* light = sceneLights[i];
		LightType lightType = light->GetLightType();
	
		if (lightType == LT_DirectionalLight)
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

			if (lightType == LT_SpotLight)
				DrawSpotLightShape(light, tech);
			else if (lightType == LT_PointLight)
				DrawPointLightShape(light, tech);
		}
	}
}

void DeferredPath::DeferredShading()
{
	mDevice->BindFrameBuffer(mHDRFB);
	mHDRBufferRTV->ClearColor(ColorRGBA(0, 0, 0, 0));

	// Draw Sky box first
	mSceneMan->UpdateBackgroundQueue(*(mGBufferFB->GetCamera()));
	RenderBucket& bkgBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketBackground, false);
	for (RenderQueueItem& item : bkgBucket)
		item.Renderable->Render();

	// Do deferred lighting shading pass
	mDeferedMaterial->SetTexture("GBuffer0", mGBuffer[0]->GetShaderResourceView());
	mDeferedMaterial->SetTexture("GBuffer1", mGBuffer[1]->GetShaderResourceView());
	mDeferedMaterial->SetTexture("LightAccumulateBuffer", mLightAccumulateBuffer->GetShaderResourceView());

	DrawFSQuad(mDeferedMaterial, "Shading");
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

	float3 lightColor = light->GetLightColor() * light->GetLightIntensity();
	mDeferredEffect->GetParameterByUsage(EPU_Light_Color)->SetValue(lightColor);

	const float3& worldDirection = light->GetDerivedDirection();
	float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
	lightDir = lightDir * currCamera.GetViewMatrix();
	mDeferredEffect->GetParameterByUsage(EPU_Light_Dir)->SetValue(lightDir);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mDeferredEffect->GetParameterByName("CameraNearFar")->SetValue(camNearFar);
	mDeferredEffect->GetParameterByName("ShadowEnabled")->SetValue(bCastShadow);
	if (bCastShadow)
	{	
		mDeferredEffect->GetParameterByName("CascadeShadowTex")->SetValue(mShadowMan->mShadowTexture->GetShaderResourceView());
		mDeferredEffect->GetParameterByName("ShadowView")->SetValue(mShadowMan->mShadowView);
		mDeferredEffect->GetParameterByName("NumCascades")->SetValue((int)light->GetShadowCascades());
		mDeferredEffect->GetParameterByName("BorderPaddingMinMax")->SetValue(mShadowMan->mBorderPaddingMinMax);
		mDeferredEffect->GetParameterByName("CascadeScale")->SetValue(&(mShadowMan->mShadowCascadeScale[0]), MAX_CASCADES);
		mDeferredEffect->GetParameterByName("CascadeOffset")->SetValue(&(mShadowMan->mShadowCascadeOffset[0]), MAX_CASCADES); 
		//effect->GetParameterByName("CascadeBlendArea")->SetValue(mCascadedShadowMap->mCascadeBlendArea);
	}

	String techName = "Directional" + tech;
	DrawFSQuad(mDeferedMaterial, techName);
}

void DeferredPath::DrawSpotLightShape( Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());

	bool bCastShadow = light->GetCastShadow();
	if (bCastShadow)
		mShadowMan->MakeSpotShadowMap(*light);

	float3 lightColor = light->GetLightColor() * light->GetLightIntensity();
	const float3& worldPos = light->GetDerivedPosition();
	const float3& worldDir = light->GetDerivedDirection();

	float4 lightPos = float4(worldPos[0], worldPos[1], worldPos[2], 1.0f) * currCamera.GetViewMatrix();
	float4 lightDir = float4(worldDir[0], worldDir[1], worldDir[2], 0.0f) * currCamera.GetViewMatrix();
	
	float spotInnerAngle = light->GetSpotInnerAngle();
	float spotOuterAngle = light->GetSpotOuterAngle();
	lightPos[3] = cosf(spotInnerAngle);
	lightDir[3] = cosf(spotOuterAngle);

	mDeferredEffect->GetParameterByUsage(EPU_Light_Color)->SetValue(lightColor);
	mDeferredEffect->GetParameterByUsage(EPU_Light_Position)->SetValue(lightPos);
	mDeferredEffect->GetParameterByUsage(EPU_Light_Dir)->SetValue(lightDir);
	mDeferredEffect->GetParameterByUsage(EPU_Light_Attenuation)->SetValue(light->GetAttenuation());

	float scaleHeight = light->GetRange();
	float scaleBase = scaleHeight * tanf(spotOuterAngle * 0.5f);

	float3 rotAxis = Cross(float3(0, 1, 0), worldDir);
	//float rotAngle = acosf(Dot(float3(0, 1, 0), worldDirection));
	float4x4 rotation = CreateRotationAxis(rotAxis, acosf(worldDir.Y()));

	float4x4 worldMatrix = CreateScaling(scaleBase, scaleHeight, scaleBase) * rotation *
						   CreateTranslation(worldPos);
	
	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mDeferredEffect->GetParameterByName("CameraNearFar")->SetValue(camNearFar);
	mDeferredEffect->GetParameterByName("ShadowEnabled")->SetValue(bCastShadow);

	if (bCastShadow)
	{
		/*TextureLayer shadewTexLayer;
		shadewTexLayer.Sampler = mShadowMan->mPCFSampleState;
		shadewTexLayer.Stage = ST_Pixel;
		shadewTexLayer.Texture = mShadowMan->mShadowDepth;
		shadewTexLayer.TexUnit = 5; */
		
		mDeferredEffect->GetParameterByName("ShadowTex")->SetValue(mShadowMan->mShadowDepth->GetShaderResourceView());
		mDeferredEffect->GetParameterByName("ShadowViewProj")->SetValue(mShadowMan->mShadowView);
		if (mDeferredEffect->GetParameterByName("PCFRadius"))
			mDeferredEffect->GetParameterByName("PCFRadius")->SetValue(5.7f / float(SHADOW_MAP_SIZE));
	}

	String techName = "Spot" + tech;
	mDeferedMaterial->ApplyMaterial(worldMatrix);
	mDeferedMaterial->SetCurrentTechnique("Spot" + tech);
	mDevice->Draw(mDeferedMaterial->GetCurrentTechnique(), *mSpotLightShape);
}

void DeferredPath::DrawPointLightShape(Light* light, const String& tech )
{
	const Camera& currCamera = *(mDevice->GetCurrentFrameBuffer()->GetCamera());

	float lightRadius = light->GetRange();

	float3 lightColor = light->GetLightColor() * light->GetLightIntensity();
	mDeferredEffect->GetParameterByUsage(EPU_Light_Color)->SetValue(lightColor);

	float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
	mDeferedMaterial->GetEffect()->GetParameterByName("CameraNearFar")->SetValue(camNearFar);

	const float3& worldPos = light->GetDerivedPosition();
	float4 lightPos(worldPos[0], worldPos[1], worldPos[2], 1.0f);
	lightPos = lightPos * currCamera.GetViewMatrix();
	mDeferredEffect->GetParameterByUsage(EPU_Light_Position)->SetValue(lightPos);

	mDeferredEffect->GetParameterByUsage(EPU_Light_Attenuation)->SetValue(light->GetAttenuation());

	float4x4 worldMatrix(lightRadius,  0.0f,		  0.0f,		    0.0f,
		0.0f,         lightRadius,   0.0f,		    0.0f,
		0.0f,         0.0f,          lightRadius,  0.0f,
		worldPos.X(), worldPos.Y(),  worldPos.Z(), 1.0f);

	mDeferedMaterial->ApplyMaterial(worldMatrix);
	mDeferedMaterial->SetCurrentTechnique("Point" + tech);
	mDevice->Draw(mDeferedMaterial->GetCurrentTechnique(), *mPointLightShape);
}

}