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

	for (size_t iStage = 0; iStage < pipeline->mPipelineStages.size(); ++iStage)
	{
		Pipeline::PipelineStage& stage = pipeline->mPipelineStages[iStage];
		if( !stage.Enabled ) continue;

		for( size_t iCommand = 0; iCommand < stage.Commands.size(); ++iCommand )
		{
			Pipeline::PipelineCommand& command = stage.Commands[iCommand];
			
			switch(command.Command)
			{
			case Pipeline::BindFrameBuffer:
				{
					const String& fbName = command.params[0].GetParamString();
					shared_ptr<FrameBuffer> frameBuffer = pipeline->GetFrameBuffer(fbName);

					if (!frameBuffer)
					{
						frameBuffer = device.GetScreenFrameBuffer();
					}

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

void Renderer::SetupViewProjMatirices( const Matrix4f& view, const Matrix4f& proj )
{
	mViewMat = view;
	mViewMatInv = view.Inverse();
	mProjMat = proj;
	mViewProjMat = view * proj;
}

}