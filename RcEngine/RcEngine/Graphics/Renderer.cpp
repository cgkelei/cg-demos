#include <Graphics/Renderer.h>
#include <Graphics/Pipeline.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderView.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Material.h>
#include <Core/Context.h>

namespace RcEngine {

Renderer::Renderer()
	: mCurrentFrameBuffer(nullptr), mCurrentLight(nullptr), mCurrentTechnique(nullptr),
	  mCurrentMat(nullptr)
{

}

Renderer::~Renderer()
{

}

void Renderer::DrawFSQuad( const shared_ptr<Material>& mat, const String& tech )
{

}

void Renderer::DrawGeometry( const String& tech, const  String& matClass)
{

}

void Renderer::DrawRenderable( Camera* camera, const String& tech, const  String& matClass)
{

}

void Renderer::RenderScene( const shared_ptr<Pipeline>& pipeline )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	for (Pipeline::PipelineStage& stage : pipeline->mPipelineStages)
	{
		if( !stage.Enabled ) continue;

		for( Pipeline::PipelineCommand& command : stage.Commands)
		{
			switch(command.Command)
			{
			case Pipeline::BindFrameBuffer:
				{
					const String& fbName = command.params[0].GetParamString();
					shared_ptr<FrameBuffer> frameBuffer = pipeline->GetFrameBuffer(fbName);

					if (!frameBuffer)
						frameBuffer = device.GetScreenFrameBuffer();

					device.BindFrameBuffer(frameBuffer);
					mCurrentFrameBuffer = frameBuffer;
				}
				break;

			case Pipeline::ClearFrameBuffer:
				{
					Attachment att = (Attachment)command.params[0].GetParamUInt();
					if (att == ATT_DepthStencil)
					{
						float depth = command.params[1].GetParamFloat();
						uint32_t stencil = command.params[2].GetParamUInt();
						mCurrentFrameBuffer->GetAttachedView(att)->ClearDepthStencil(depth, stencil);
					}
					else
					{
						const ColorRGBA& clearColor = command.params[1].GetParamColor();
						mCurrentFrameBuffer->GetAttachedView(att)->ClearColor(clearColor);
					}
				}
				break;

			case Pipeline::SwapBuffer:
				{
					mCurrentFrameBuffer->SwapBuffers();
				}
				break;

			case Pipeline::DrawGeometry:
				{
					const String& techName = command.params[0].GetParamString();
					const String& matClass = command.params[1].GetParamString();
					
					RenderOrder order = RO_None;
					if (command.params.size() > 2 )
					{
						order = RenderOrder(command.params[2].GetParamUInt());
					}

					DrawGeometry(techName, matClass);
				}
				break;
			}
		}
	}
}

void Renderer::UpdateShadowMap()
{

}

void Renderer::SetupViewProjMatirices( const float4x4& view, const float4x4& proj )
{
	mViewMat = view;
	mViewMatInv = view.Inverse();
	mProjMat = proj;
	mViewProjMat = view * proj;
}

}