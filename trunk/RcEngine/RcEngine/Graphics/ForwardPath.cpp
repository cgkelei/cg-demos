#include <Graphics/RenderPath.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/Effect.h>
#include <Graphics/Material.h>
#include <Scene/SceneManager.h>
#include <Scene/Light.h>
#include <MainApp/Window.h>
#include <MainApp/Application.h>
#include <Resource/ResourceManager.h>
#include <fstream>

namespace RcEngine {

ForwardPlusPath::ForwardPlusPath()
{

}

ForwardPlusPath::~ForwardPlusPath()
{

}

void ForwardPlusPath::OnGraphicsInit( const shared_ptr<Camera>& camera )
{
	RenderPath::OnGraphicsInit(camera);

	RenderFactory* factory = mDevice->GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	mTiledLightCullEfffect = resMan.GetResourceByName<Effect>(RT_Effect, "TiledLightCull.effect.xml", "General");
	mTileLightCullTech = mTiledLightCullEfffect->GetTechniqueByName("TiledLightCull");

	Window* appWindow = Application::msApp->GetMainWindow();
	const uint32_t windowWidth = appWindow->GetWidth();
	const uint32_t windowHeight = appWindow->GetHeight();

#ifdef _DEBUG
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read;
#else
	uint32_t acessHint = EAH_GPU_Write | EAH_GPU_Read;
#endif

	uint32_t rtCreateFlag = TexCreate_ShaderResource | TexCreate_RenderTarget;
	uint32_t dsCreateFlag = TexCreate_ShaderResource | TexCreate_DepthStencilTarget;

	mDepthStencilBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_D24S8, 1, 1, 1, 0, acessHint, dsCreateFlag, NULL);
	mHDRBuffer = factory->CreateTexture2D(windowWidth, windowHeight, PF_RGBA32F, 1, 1, 1, 0, acessHint, rtCreateFlag, NULL);

	mForwardFB = factory->CreateFrameBuffer(windowWidth, windowHeight);
	mForwardFB->SetViewport(Viewport(0.0f, 0.f, float(windowWidth), float(windowHeight)));
	mForwardFB->SetCamera(camera);

	mDepthStencilView = factory->CreateDepthStencilView(mDepthStencilBuffer, 0, 0);
	mHDRBufferRTV = factory->CreateRenderTargetView2D(mHDRBuffer, 0, 0);

	mForwardFB->AttachRTV(ATT_Color0, mHDRBufferRTV);
	mForwardFB->AttachRTV(ATT_DepthStencil, mDepthStencilView);

	// Light Buffer
	mPointLightsPosRange = factory->CreateTextureBuffer(PF_RGBA32F, MaxNumLights, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Texture, NULL);
	mPointLightsColorFalloff = factory->CreateTextureBuffer(PF_RGB32F, MaxNumLights*2, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Texture, NULL);

	mPointLightsPosRangeSRV = factory->CreateTextureBufferSRV(mPointLightsPosRange, 0, MaxNumLights,  PF_RGBA32F);
	mPointLightsColorSRV = factory->CreateTextureBufferSRV(mPointLightsColorFalloff, 0, MaxNumLights,  PF_RGB32F);
	mPointLightsFalloffSRV = factory->CreateTextureBufferSRV(mPointLightsColorFalloff, MaxNumLights, MaxNumLights,  PF_RGB32F);

	mNumTileX= (windowWidth + TileGroupSize - 1) / TileGroupSize;
	mNumTileY = (windowHeight + TileGroupSize - 1) / TileGroupSize;
	uint32_t numTotalTiles = mNumTileX * mNumTileY;

	mPointLightsIndexCounter = factory->CreateTextureBuffer(PF_R32U, 1, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Texture, NULL);
	mTilePointLightsIndexList = factory->CreateTextureBuffer(PF_R32U, numTotalTiles*MaxNumLightsPerTile, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Texture, NULL);
	mTilePointLightsRange = factory->CreateTextureBuffer(PF_RG32U, numTotalTiles, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Texture, NULL);
	
	mPointLightsIndexCounterUAV = factory->CreateTextureBufferUAV(mPointLightsIndexCounter, 0, 1, PF_R32U);
	
	mTilePointLightsRangeSRV = factory->CreateTextureBufferSRV(mTilePointLightsRange, 0, numTotalTiles, PF_R32U);
	mTilePointLightsRangeUAV = factory->CreateTextureBufferUAV(mTilePointLightsRange, 0, numTotalTiles, PF_R32U);

	mTilePointLightsIndexListSRV = factory->CreateTextureBufferSRV(mTilePointLightsIndexList, 0, numTotalTiles*MaxNumLightsPerTile, PF_R32U);
	mTilePointLightsIndexListUAV = factory->CreateTextureBufferUAV(mTilePointLightsIndexList, 0, numTotalTiles*MaxNumLightsPerTile, PF_R32U);

	mTiledLightCullEfffect->GetParameterByName("DepthBuffer")->SetValue(mDepthStencilBuffer->GetShaderResourceView());
	mTiledLightCullEfffect->GetParameterByName("PointLightsPosRange")->SetValue(mPointLightsPosRangeSRV);
	mTiledLightCullEfffect->GetParameterByName("RWLightIndexCounter")->SetValue(mPointLightsIndexCounterUAV);
	mTiledLightCullEfffect->GetParameterByName("RWLightIndexList")->SetValue(mTilePointLightsIndexListUAV);
	mTiledLightCullEfffect->GetParameterByName("RWLightListRange")->SetValue(mTilePointLightsRangeUAV);
}


void ForwardPlusPath::OnWindowResize( uint32_t width, uint32_t height )
{

}

void ForwardPlusPath::RenderScene()
{
	DepthPrePass();
	TiledLightCulling();
	ForwardShading();
}

void ForwardPlusPath::DepthPrePass()
{
	mDevice->BindFrameBuffer(mForwardFB);
	mDepthStencilView->ClearDepthStencil(0.0f, 0);

	// Todo: update render queue with render bucket filter
	mSceneMan->UpdateRenderQueue(*mCamera, RO_None);   

	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	
	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		renderItem.Renderable->GetMaterial()->SetCurrentTechnique("DepthPre");
		renderItem.Renderable->Render();
	}

	//auto proj = mCamera->GetProjMatrix();
	//mDevice->GetRenderFactory()->SaveLinearDepthTextureToFile("E:/Depth.pfm", mDepthStencilBuffer, proj.M33, proj.M43);
}

void ForwardPlusPath::TiledLightCulling()
{
	uint32_t windowWidth = mHDRBuffer->GetWidth();
	uint32_t windowHeight = mHDRBuffer->GetHeight();

	mSceneMan->UpdateLightQueue(*mCamera);
	const LightQueue& sceneLights = mSceneMan->GetLightQueue();

	float4* pLightsPosRange = reinterpret_cast<float4*>( mPointLightsPosRange->Map(0, sizeof(float4) * sceneLights.size(), RMA_Write_Discard) );
	float3* pLightsColor = reinterpret_cast<float3*>( mPointLightsColorFalloff->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard) );
	float3* pLightsFalloff = pLightsColor + sizeof(float3) * MaxNumLights;

	uint32_t numTotalCount = 0;
	for (Light* light : sceneLights)
	{
		if (light->GetLightType() == LT_PointLight)
		{
			const float3 lightPos = light->GetPosition();
			pLightsPosRange[numTotalCount] = float4(lightPos[0], lightPos[1], lightPos[2], light->GetRange());
			pLightsColor[numTotalCount] = light->GetLightColor() * light->GetLightIntensity();
			pLightsFalloff[numTotalCount] = light->GetAttenuation();

			numTotalCount++;
		}
	}
	mPointLightsPosRange->UnMap();
	mPointLightsColorFalloff->UnMap();

	const float4x4& view = mCamera->GetViewMatrix();
	const float4x4& proj = mCamera->GetProjMatrix();
	const float4x4 invViewProj = MatrixInverse(view * proj);
	const float4x4 invProj = MatrixInverse(proj);
	float4 viewDim = float4((float)windowWidth, (float)windowHeight, 1.0f / windowWidth, 1.0f / windowHeight);

	mTiledLightCullEfffect->GetParameterByName("View")->SetValue(view);
	mTiledLightCullEfffect->GetParameterByName("Projection")->SetValue(proj);
	mTiledLightCullEfffect->GetParameterByName("InvProj")->SetValue(invProj);
	mTiledLightCullEfffect->GetParameterByName("InvViewProj")->SetValue(invViewProj);
	mTiledLightCullEfffect->GetParameterByName("ViewportDim")->SetValue(viewDim);
	mTiledLightCullEfffect->GetParameterByName("ProjRatio")->SetValue(float2(proj.M33, proj.M43));
	mTiledLightCullEfffect->GetParameterByName("LightCount")->SetValue(numTotalCount);

	// Reset Counter
	uint32_t* pCounter = reinterpret_cast<uint32_t*>( mPointLightsIndexCounter->Map(0, MAP_ALL_BUFFER, RMA_Write_Discard) );
	*pCounter = 0;
	mPointLightsIndexCounter->UnMap();

	mDevice->DispatchCompute(mTileLightCullTech, mNumTileX, mNumTileY, 1);

	uint2* pRange = reinterpret_cast<uint2*>( mTilePointLightsRange->Map(0, MAP_ALL_BUFFER, RMA_Read_Only) );
	
	FILE* pFile = fopen("E:/TileLightRange.txt", "w");
	
	for (int32_t iY = mNumTileY-1; iY >= 0; iY--)
	{
		for (int32_t iX = 0; iX < mNumTileX; iX++)
		{
			int32_t index = iY * mNumTileX + iX;
			fprintf(pFile, "(%d, %d) ", pRange[index].X(), pRange[index].Y());
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);

	mTilePointLightsRange->UnMap();
}

void ForwardPlusPath::ForwardShading()
{
	mDevice->BindFrameBuffer(mForwardFB);
	mHDRBufferRTV->ClearColor(ColorRGBA(0, 0, 0, 0)); 

	RenderBucket& opaqueBucket = mSceneMan->GetRenderQueue().GetRenderBucket(RenderQueue::BucketOpaque);	
	for (const RenderQueueItem& renderItem : opaqueBucket) 
	{
		shared_ptr<Effect> finalShadingEffect = renderItem.Renderable->GetMaterial()->GetEffect();

		finalShadingEffect->GetParameterByName("CameraOrigin")->SetValue(mCamera->GetPosition());
		finalShadingEffect->GetParameterByName("PointLightsPosRange")->SetValue(mPointLightsPosRangeSRV);
		finalShadingEffect->GetParameterByName("PointLightsColor")->SetValue(mPointLightsColorSRV);
		finalShadingEffect->GetParameterByName("PointLightsFalloff")->SetValue(mPointLightsFalloffSRV);
		finalShadingEffect->GetParameterByName("LightIndexList")->SetValue(mTilePointLightsIndexListSRV);
		finalShadingEffect->GetParameterByName("LightListRange")->SetValue(mTilePointLightsRangeSRV);
		finalShadingEffect->SetCurrentTechnique("ForwardShading");
		renderItem.Renderable->Render();
	}

	mDevice->GetRenderFactory()->SaveTextureToFile("E:/HDR.pfm", mHDRBuffer);
}



}