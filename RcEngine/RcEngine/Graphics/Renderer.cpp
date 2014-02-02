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
#include <Scene/SceneManager.h>
#include <Graphics/GraphicBuffer.h>
#include <GUI/UIManager.h>
#include <Core/Exception.h>
#include <Scene/Light.h>
#include <Core/Context.h>
#include <Math/MathUtil.h>

namespace {

using namespace RcEngine;

shared_ptr<RenderOperation> BuildFSQuadShape()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	// Fullscreen polygon
	float vertices[6] = {  // x, y, z
		-1.0f, 1.0f, -1.0f, -3.0f, 3.0f, 1.0f   // Front Face CCW, todo unify OpenGL and D3D back face culling
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

	const int nRings = 200;
	const int nSegments = 200;
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

		//shared_ptr<RenderOperation> BuildSpotLightShape() 
		//{
		//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();


		//	VertexElement vdsc[] = {
		//		VertexElement(0, VEF_Float3,  VEU_Position, 0),
		//	};


		//	float mRadius = 1.f;
		//	float mHeight = 1.f;
		//	unsigned int mNumSegBase = 16;
		//	unsigned int mNumSegHeight = 1;

		//	uint32_t vertexCount = (mNumSegHeight+1)*(mNumSegBase+1)+mNumSegBase+2;
		//	uint32_t indicesCount = mNumSegHeight*mNumSegBase*6+3*mNumSegBase;

		//	ElementInitData vInitData;
		//	vInitData.pData = nullptr;
		//	vInitData.rowPitch = 3 * vertexCount * sizeof(float);
		//	vInitData.slicePitch = 0;
		//	shared_ptr<GraphicsBuffer> vertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

		//	ElementInitData iInitData;
		//	iInitData.pData = nullptr;
		//	iInitData.rowPitch = indicesCount * sizeof(uint16_t);
		//	iInitData.slicePitch = 0;
		//	shared_ptr<GraphicsBuffer> indexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

		//	float* pVertex = static_cast<float*>(vertexBuffer->Map(0, -1, BA_Write_Only));
		//	uint16_t* pIndices = static_cast<uint16_t*>(indexBuffer->Map(0, -1, BA_Write_Only));

		//	float deltaAngle = (Mathf::TWO_PI / mNumSegBase);
		//	float deltaHeight = mHeight/(float)mNumSegHeight;
		//	int offset = 0;

		//	for (uint32_t i = 0; i <=mNumSegHeight; i++)
		//	{
		//		float r0 = mRadius * i / (float)mNumSegHeight;
		//		for (uint32_t j = 0; j<=mNumSegBase; j++)
		//		{
		//			float x0 = r0* cosf(j*deltaAngle);
		//			float y0 = r0 * sinf(j*deltaAngle);

		//			// Add one vertex to the strip which makes up the sphere
		//			*pVertex++ = x0;
		//			*pVertex++ = y0;
		//			*pVertex++ = i*deltaHeight;
		//	
		//			if (i != mNumSegHeight&& j != mNumSegBase)
		//			{
		//				*pIndices++ = offset + mNumSegBase + 2;
		//				*pIndices++ = offset;
		//				*pIndices++ = offset + mNumSegBase+1;

		//				*pIndices++ = offset + mNumSegBase + 2;
		//				*pIndices++ = offset + mNumSegBase+1;
		//				*pIndices++ = offset;
		//			}

		//			offset ++;
		//		}
		//	}

		//	//low cap
		//	int centerIndex = offset;

		//	*pVertex++ = 0.0f;
		//	*pVertex++ = 0.0f;
		//	*pVertex++ = mHeight;

		//	offset++;
		//	for (uint32_t j=0; j<=mNumSegBase; j++)
		//	{
		//		float x0 = mRadius * cosf(j*deltaAngle);
		//		float y0 = mRadius * sinf(j*deltaAngle);

		//		*pVertex++ = x0;
		//		*pVertex++ = y0;
		//		*pVertex++ = mHeight;

		//		if (j!=mNumSegBase)
		//		{
		//			*pIndices++ = centerIndex;
		//			*pIndices++ = offset;
		//			*pIndices++ = offset+1;
		//		}
		//		offset++;
		//	}
		//}

}



namespace RcEngine {

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
	RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();
	if (!device)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Create device first!", "Renderer::Init");
		return;
	}

	mPointLightShape = BuildPointLightShape();
	mFSQuadShape = BuildFSQuadShape();
}


void Renderer::CreatePrimitives()
{

}



void Renderer::SetRenderPipeline( const shared_ptr<Pipeline>& pipeline )
{
	mCurrPipeline = pipeline;
}

void Renderer::DrawRenderable( Camera* camera, const String& tech, const  String& matClass)
{

}


void Renderer::RenderScene()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	for (Pipeline::PipelineStage& stage : mCurrPipeline->mPipelineStages)
	{
		if( !stage.Enabled ) continue;

		for( const Pipeline::PipelineCommand* command : stage.Commands)
		{
			switch(command->Command)
			{
			case Pipeline::SwitchFrameBuffer:
				{
					int32_t fbIndex = command->Params[0].GetInt();
					shared_ptr<FrameBuffer> frameBuffer;

					if (fbIndex == -1) // default
						frameBuffer = device.GetScreenFrameBuffer();
					else
						frameBuffer = mCurrPipeline->GetFrameBuffer(fbIndex);
					
					device.BindFrameBuffer(frameBuffer);
				}
				break;

			case Pipeline::ClearTarget:
				{
					Attachment att = (Attachment)command->Params[0].GetInt();
					if (att == ATT_DepthStencil)
					{
						if (command->Params.size() == 2)
						{
							float depth = command->Params[1].GetFloat();
							device.GetCurrentFrameBuffer()->GetAttachedView(ATT_DepthStencil)->ClearDepth(depth);
						}
						else
						{
							float depth = command->Params[1].GetFloat();
							int32_t stencil = command->Params[2].GetInt();
							device.GetCurrentFrameBuffer()->GetAttachedView(ATT_DepthStencil)->ClearDepthStencil(depth, stencil);
						}	
					}
					else
					{
						const ColorRGBA& clearColor = command->Params[1].GetColor();
						device.GetCurrentFrameBuffer()->GetAttachedView(att)->ClearColor(clearColor);
					}
				}
				break;

			case Pipeline::DrawGeometry:
				{
					const String& techName = command->Params[0].GetString();
					//const String& matClass = command->Params[1].GetString();
					String matClass = "";

					RenderOrder order = RO_None;
					if (command->Params.size() > 2 )
						order = RenderOrder(command->Params[2].GetInt());

					DrawGeometry(techName, matClass, order);
				}
				break;

			case Pipeline::DrawLightShape:
				{
					int32_t matIdx = command->Params[0].GetInt();
					const String& techName = command->Params[1].GetString();

					// store for later use
					mCurrMaterial = mCurrPipeline->GetMaterial(matIdx);

					for (Pipeline::PipelineCommand* cmd : command->SubCommands)
						UpdateMaterialParameters(cmd);

					DrawLightShape(techName);
				}
				break;

			case Pipeline::DrawQuad:
				{
					int32_t matIdx = command->Params[0].GetInt();
					const String& techName = command->Params[1].GetString();

					// store for later use
					mCurrMaterial = mCurrPipeline->GetMaterial(matIdx);

					for (Pipeline::PipelineCommand* cmd : command->SubCommands)
						UpdateMaterialParameters(cmd);

					DrawFSQuad(techName);

					//const shared_ptr<Texture>& rt = mCurrPipeline->GetRenderTarget(2, 0);
					//device.GetRenderFactory()->SaveTexture2D("E:/DeferredShading.pfm", rt, 0, 0);
				}
				break;

			case Pipeline::DrawOverlays:
				{
					DrawOverlays();
				}
				break;
			}
		
		}
	}

	device.GetScreenFrameBuffer()->SwapBuffers();
}

void Renderer::UpdateMaterialParameters( Pipeline::PipelineCommand* cmd )
{
	if (cmd->Command == Pipeline::BindBuffer)
	{
		const String& targetName = cmd->Params[0].GetString();
		int32_t pipeFrameBufferIdx = cmd->Params[1].GetInt();
		int32_t pipeTextureIndex = cmd->Params[2].GetInt();

		const shared_ptr<Texture>& rt = mCurrPipeline->GetRenderTarget(pipeFrameBufferIdx, pipeTextureIndex);
		mCurrMaterial->SetTexture(targetName, rt);
	}
	else if (cmd->Command == Pipeline::SetUniform)
	{
		
	}
}

void Renderer::UpdateShadowMap()
{

}

void Renderer::DrawGeometry( const String& tech, const  String& matClass, RenderOrder order)
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	const Camera& camera = *(device.GetCurrentFrameBuffer()->GetCamera());	
	sceneMan.UpdateRenderQueue(camera, order);

	RenderQueue* renderQueue = sceneMan.GetRenderQueue();	
	for (const auto& kv : renderQueue->GetAllRenderBuckets())
	{
		RenderBucket& renderBucket = *(kv.second);

		if (renderBucket.size())
		{
			std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
				return lhs.SortKey < rhs.SortKey; });

				for (const RenderQueueItem& renderItem : renderBucket) 
				{
					renderItem.Renderable->GetMaterial()->SetCurrentTechnique(tech);
					renderItem.Renderable->Render();
				}
		}


	}

	//const shared_ptr<Texture>& rt0 = mCurrPipeline->GetRenderTarget(0, 1);
	//const shared_ptr<Texture>& rt1 = mCurrPipeline->GetRenderTarget(0, 2);
	//device.GetRenderFactory()->SaveTexture2D("E:/GBuffer0.tga", rt0, 0, 0);
	//device.GetRenderFactory()->SaveTexture2D("E:/GBuffer1.tga", rt1, 0, 0);
}

void Renderer::DrawLightShape( const String& tech )
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	const Camera& currCamera = *(device.GetCurrentFrameBuffer()->GetCamera());

	// Fetch all lights affect current view frustum
	const std::vector<Light*>& lights = sceneMan.GetSceneLights();

	for (Light* light : lights)
	{
		const float3& lightColor = light->GetLightColor();
		mCurrMaterial->GetCustomParameter(EPU_Light_Color)->SetValue(lightColor);

		if (light->GetLightType() == LT_Directional)
		{
			const float3& worldDirection = light->GetDerivedDirection();
			float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
			lightDir = lightDir * currCamera.GetViewMatrix();

			mCurrMaterial->GetCustomParameter(EPU_Light_Dir)->SetValue(lightDir);
			
			float2 camNearFar(currCamera.GetNearPlane(), currCamera.GetFarPlane());
			mCurrMaterial->GetEffect()->GetParameterByName("CameraNearFar")->SetValue(camNearFar);

			String techName = "Directional" + tech;
			DrawFSQuad(techName);
		}
		else if (light->GetLightType() == LT_Point)
		{
			float attenuationRange = light->GetAttenuationRange();

			const float3& worldPosition = light->GetDerivedPosition();
			float4 lightPos(worldPosition[0], worldPosition[1], worldPosition[2], 1.0f);
			lightPos = lightPos * currCamera.GetViewMatrix();
			mCurrMaterial->GetCustomParameter(EPU_Light_Position)->SetValue(lightPos);

			float3 lightFalloff(light->GetAttenuationConstant(), light->GetAttenuationLinear(), light->GetAttenuationQuadric());
			mCurrMaterial->GetCustomParameter(EPU_Light_Falloff)->SetValue(lightFalloff);
			
			String techName = "Point" + tech;
			DrawPointLightShape(worldPosition, attenuationRange, techName);
		}
		else if (light->GetLightType() == LT_SpotLight)
		{
			float attenuationRange = light->GetAttenuationRange();

			const float3& worldPosition = light->GetDerivedDirection();
			float4 lightPos(worldPosition[0], worldPosition[1], worldPosition[2], 1.0f);
			lightPos = lightPos * currCamera.GetViewMatrix();
			
			const float3& worldDirection = light->GetDerivedDirection();
			float4 lightDir(worldDirection[0], worldDirection[1], worldDirection[2], 0.0f);
			lightDir = lightDir * currCamera.GetViewMatrix();

			lightPos[4] = cosf(light->GetSpotlightInnerAngle());
			lightDir[4] = cosf(light->GetSpotlightOuterAngle());
			mCurrMaterial->GetCustomParameter(EPU_Light_Dir)->SetValue(lightDir);
			mCurrMaterial->GetCustomParameter(EPU_Light_Position)->SetValue(lightPos);

			float3 lightFalloff(light->GetAttenuationConstant(), light->GetAttenuationLinear(), light->GetAttenuationQuadric());
			mCurrMaterial->GetCustomParameter(EPU_Light_Falloff)->SetValue(lightFalloff);

			String techName = "Spot" + tech;
			//DrawCone()
		}
	}

	{
		const shared_ptr<Texture>& rt = mCurrPipeline->GetRenderTarget(1, 0);
		device.GetRenderFactory()->SaveTexture2D("E:/DeferredLight.pfm", rt, 0, 0);
	}
}

void Renderer::DrawPointLightShape( const float3& worldPos, float radius, const String& tech )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	float4x4 worldMatrix = CreateScaling(radius, radius, radius) * CreateTranslation(worldPos);

	mCurrMaterial->ApplyMaterial(worldMatrix);
	mCurrMaterial->SetCurrentTechnique(tech);
	device.Render(*mCurrMaterial->GetCurrentTechnique(), *mPointLightShape);
}

void Renderer::DrawFSQuad( const String& tech )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	mCurrMaterial->ApplyMaterial();
	mCurrMaterial->SetCurrentTechnique(tech);
	device.Render(*mCurrMaterial->GetCurrentTechnique(), *mFSQuadShape);
}

void Renderer::DrawOverlays()
{
	// Update overlays
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	UIManager& uiMan = UIManager::GetSingleton();

	uiMan.Render();
	sceneMan.UpdateOverlayQueue();

	RenderBucket& guiBucket = sceneMan.GetRenderQueue()->GetRenderBucket(RenderQueue::BucketOverlay);	
	if (guiBucket.size())
	{
		std::sort(guiBucket.begin(), guiBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
			return lhs.SortKey < rhs.SortKey; });

			for (const RenderQueueItem& renderItem : guiBucket) 
				renderItem.Renderable->Render();
	}
}





}