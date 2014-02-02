#include "OpenGLRenderDevice.h"
#include "OpenGLRenderFactory.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderWindow.h"
#include "OpenGLRenderView.h"
#include "OpenGLSamplerState.h"
#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/Material.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectPass.h>
#include <Graphics/Viewport.h>
#include <Graphics/BlendState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/BlendState.h>
#include <Graphics/RenderOperation.h>
#include <Core/Exception.h>
#include <Core/Context.h>
#include <Math/MathUtil.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace RcEngine {

OpenGLRenderDevice::OpenGLRenderDevice()
	: mViewportTop(0), mViewportLeft(0), mViewportWidth(0), mViewportHeight(0)
{
	mRenderDeviceType = RD_OpenGL;
}


OpenGLRenderDevice::~OpenGLRenderDevice(void)
{
	SAFE_DELETE(mRenderFactory);
}

void OpenGLRenderDevice::Create()
{
	mRenderFactory = new OpenGLRenderFactory();
	Context::GetSingleton().SetRenderFactory(mRenderFactory);
}

void OpenGLRenderDevice::Release()
{
	SAFE_DELETE(mRenderFactory);
}

void OpenGLRenderDevice::CreateRenderWindow( const RenderSettings& settings )
{
	mRenderSettings = settings;

	mScreenFrameBuffer = std::make_shared<OpenGLRenderWindow>(settings);
	
	BindFrameBuffer(mScreenFrameBuffer);
	
	mScreenFrameBuffer->Attach(ATT_Color0, 
		std::make_shared<OpenGLScreenRenderTargetView2D>(mScreenFrameBuffer->GetWidth(), mScreenFrameBuffer->GetHeight(), mScreenFrameBuffer->GetColorFormat()));
			
	if(mScreenFrameBuffer->IsDepthBuffered())
	{
		mScreenFrameBuffer->Attach(ATT_DepthStencil,
			std::make_shared<OpenGLScreenDepthStencilView>(mScreenFrameBuffer->GetWidth(), mScreenFrameBuffer->GetHeight(), settings.DepthStencilFormat));
	}

	//Create default render state
	// Set default render state
	mCurrentDepthStencilState = mRenderFactory->CreateDepthStencilState(DepthStencilStateDesc());

	BlendStateDesc blendDesc;
	mCurrentBlendState = mRenderFactory->CreateBlendState(blendDesc);

	RasterizerStateDesc rasterDesc;
	mCurrentRasterizerState = mRenderFactory->CreateRasterizerState(rasterDesc);

	// do little hard code
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_CUBE_MAP);
}

void OpenGLRenderDevice::AdjustProjectionMatrix( float4x4& pOut )
{
	//修改投影矩阵，使OpenGL适应左右坐标系
	float4x4 scale = CreateScaling(1.0f, 1.0f, 2.0f);
	float4x4 translate = CreateTranslation(0.0f, 0.0f, -1.0f);
	pOut =  pOut * scale * translate;
}

void OpenGLRenderDevice::BindVertexBufferOGL( const shared_ptr<GraphicsBuffer>& buffer )
{
	shared_ptr<OpenGLGraphicsBuffer> pBuffer = std::static_pointer_cast<OpenGLGraphicsBuffer>(buffer);
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer->GetBufferID());
}

void OpenGLRenderDevice::BindIndexBufferOGL( const shared_ptr<GraphicsBuffer>& indexBuffer )
{
	shared_ptr<OpenGLGraphicsBuffer> pBuffer = std::static_pointer_cast<OpenGLGraphicsBuffer>(indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->GetBufferID());
}

void OpenGLRenderDevice::DoBindFrameBuffer( const shared_ptr<FrameBuffer>& fb )
{	
	const Viewport& vp = fb->GetViewport();

	if (vp.Left != mViewportLeft || vp.Top != mViewportTop || vp.Height !=mViewportHeight || vp.Width != mViewportWidth)
	{
		glViewport(vp.Left, vp.Top, vp.Width, vp.Height);
		mViewportLeft = vp.Left;
		mViewportTop = vp.Top;
		mViewportHeight = vp.Height;
		mViewportWidth = vp.Width;
	}
}

void OpenGLRenderDevice::ToggleFullscreen( bool fs )
{

}

bool OpenGLRenderDevice::Fullscreen() const
{
	return false;
}

void OpenGLRenderDevice::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
{
	if( mCurrentBlendState != state )
	{
		const BlendStateDesc& currDesc = mCurrentBlendState->GetDesc();
		const BlendStateDesc& stateDesc = state->GetDesc();

		if (currDesc.AlphaToCoverageEnable != stateDesc.AlphaToCoverageEnable)
		{
			if (stateDesc.AlphaToCoverageEnable)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}

		if( GLEW_EXT_draw_buffers2 ) 
		{
			for(int32_t i = 0; i < 8; i++)
			{
				if(currDesc.RenderTarget[i].BlendEnable != stateDesc.RenderTarget[i].BlendEnable)
				{
					if (stateDesc.RenderTarget[i].BlendEnable)
						glEnableIndexedEXT(GL_BLEND, i);
					else
						glDisableIndexedEXT(GL_BLEND, i);
				}

				if (currDesc.RenderTarget[i].ColorWriteMask != stateDesc.RenderTarget[i].ColorWriteMask)
				{
					glColorMaskIndexedEXT(i, (stateDesc.RenderTarget[i].ColorWriteMask & CWM_Red) != 0,
						(stateDesc.RenderTarget[i].ColorWriteMask & CWM_Green) != 0,
						(stateDesc.RenderTarget[i].ColorWriteMask & CWM_Blue) != 0,
						(stateDesc.RenderTarget[i].ColorWriteMask & CWM_Alpha) != 0 );
				}

			}
		}
		else
		{
			if (currDesc.RenderTarget[0].BlendEnable != stateDesc.RenderTarget[0].BlendEnable)
			{
				if (stateDesc.RenderTarget[0].BlendEnable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}

			if (currDesc.RenderTarget[0].ColorWriteMask != stateDesc.RenderTarget[0].ColorWriteMask)
			{
				glColorMask( (stateDesc.RenderTarget[0].ColorWriteMask & CWM_Red) != 0,
					(stateDesc.RenderTarget[0].ColorWriteMask & CWM_Green) != 0,
					(stateDesc.RenderTarget[0].ColorWriteMask & CWM_Blue) != 0,
					(stateDesc.RenderTarget[0].ColorWriteMask & CWM_Alpha) != 0 );
			}
		}				

		if (currDesc.RenderTarget[0].BlendOp != stateDesc.RenderTarget[0].BlendOp)
		{
			glBlendEquationSeparate(OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOp),
				OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOpAlpha));
		}

		if ( (currDesc.RenderTarget[0].SrcBlend != stateDesc.RenderTarget[0].SrcBlend)
			|| (currDesc.RenderTarget[0].DestBlend != stateDesc.RenderTarget[0].DestBlend)
			|| (currDesc.RenderTarget[0].SrcBlendAlpha != stateDesc.RenderTarget[0].SrcBlendAlpha)
			|| (currDesc.RenderTarget[0].DestBlendAlpha != stateDesc.RenderTarget[0].DestBlendAlpha) )
		{
			//auto src1 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlend);
			//auto dst1 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlend);
			//auto src11 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlendAlpha);
			//auto dst11 = OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlendAlpha);

			glBlendFuncSeparate(OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlend), 
				                OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlend),
								OpenGLMapping::Mapping(stateDesc.RenderTarget[0].SrcBlendAlpha), 
								OpenGLMapping::Mapping(stateDesc.RenderTarget[0].DestBlendAlpha));
		}

		// Set Current
		mCurrentBlendState = state;	
	}

	if (mCurrentBlendFactor != blendFactor)
	{
		glBlendColor(blendFactor.R(), blendFactor.G(), blendFactor.B(), blendFactor.A());
		// Set Current
		mCurrentBlendFactor = blendFactor;
	}

	if(mCurrentSampleMask != sampleMask)
	{
		// Set Current
		mCurrentSampleMask = sampleMask;
	}

}

void OpenGLRenderDevice::SetRasterizerState( const shared_ptr<RasterizerState>& state )
{
	if(mCurrentRasterizerState != state)
	{
		const RasterizerStateDesc& currDesc = mCurrentRasterizerState->GetDesc();
		const RasterizerStateDesc& stateDesc = state->GetDesc();

		if (currDesc.PolygonFillMode != stateDesc.PolygonFillMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, OpenGLMapping::Mapping(stateDesc.PolygonFillMode));
		}

		if(currDesc.PolygonCullMode != stateDesc.PolygonCullMode)
		{
			switch (stateDesc.PolygonCullMode)
			{
			case CM_None:
				glDisable(GL_CULL_FACE);
				break;

			case CM_Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case CM_Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			}
		}

		if(currDesc.FrontCounterClockwise != stateDesc.FrontCounterClockwise)
		{
			glFrontFace(stateDesc.FrontCounterClockwise ? GL_CW : GL_CCW);
		}

		if ( (currDesc.DepthBias != stateDesc.DepthBias)
			|| (currDesc.SlopeScaledDepthBias != stateDesc.SlopeScaledDepthBias))
		{
			// Bias is in {0, 16}, scale the unit addition appropriately
			glPolygonOffset(stateDesc.SlopeScaledDepthBias, stateDesc.DepthBias);
		}

		if( currDesc.ScissorEnable != stateDesc.ScissorEnable )
		{
			if(stateDesc.ScissorEnable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}

		if( currDesc.MultisampleEnable != stateDesc.MultisampleEnable )
		{
			if(stateDesc.MultisampleEnable)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}

		//Set Current
		mCurrentRasterizerState = state;
	}
}

void OpenGLRenderDevice::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef, uint16_t backStencilRef )
{
	if ( (mCurrentDepthStencilState != state) || (mCurrentFrontStencilRef != frontStencilRef) 
		|| (mCurrentBackStencilRef != backStencilRef) )
	{
		const DepthStencilStateDesc& currDesc = mCurrentDepthStencilState->GetDesc();
		const DepthStencilStateDesc& stateDesc = state->GetDesc();

		if(currDesc.DepthEnable != stateDesc.DepthEnable)
		{
			if (stateDesc.DepthEnable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		if (currDesc.DepthWriteMask != stateDesc.DepthWriteMask)
		{
			glDepthMask(stateDesc.DepthWriteMask);
		}

		if (currDesc.DepthFunc != stateDesc.DepthFunc)
		{
			glDepthFunc(OpenGLMapping::Mapping(stateDesc.DepthFunc));
		}

		if(currDesc.StencilEnable != stateDesc.StencilEnable)
		{
			if (stateDesc.StencilEnable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}

		if (currDesc.StencilWriteMask != stateDesc.StencilWriteMask)
		{
			glStencilMask(stateDesc.StencilWriteMask);
		}

		if ((currDesc.FrontStencilFunc != stateDesc.FrontStencilFunc)
			|| (mCurrentFrontStencilRef != frontStencilRef)
			|| (currDesc.StencilReadMask != stateDesc.StencilReadMask))
		{
			glStencilFuncSeparate(GL_FRONT, OpenGLMapping::Mapping(stateDesc.FrontStencilFunc),
				frontStencilRef, stateDesc.StencilReadMask);
		}

		if ((currDesc.BackStencilFunc != stateDesc.BackStencilFunc)
			|| (mCurrentBackStencilRef != backStencilRef)
			|| (currDesc.StencilReadMask != stateDesc.StencilReadMask))
		{
			glStencilFuncSeparate(GL_BACK, OpenGLMapping::Mapping(stateDesc.BackStencilFunc),
				backStencilRef, stateDesc.StencilReadMask);
		}

		if ((currDesc.FrontStencilFailOp != stateDesc.FrontStencilFailOp)
			|| (currDesc.FrontStencilDepthFailOp != stateDesc.FrontStencilDepthFailOp)
			|| (currDesc.FrontStencilPassOp != stateDesc.FrontStencilPassOp))
		{
			glStencilOpSeparate(GL_FRONT, OpenGLMapping::Mapping(stateDesc.FrontStencilFailOp),
				OpenGLMapping::Mapping(stateDesc.FrontStencilDepthFailOp),
				OpenGLMapping::Mapping(stateDesc.FrontStencilPassOp) );
		}

		if ((currDesc.BackStencilFailOp != stateDesc.BackStencilFailOp)
			|| (currDesc.BackStencilDepthFailOp != stateDesc.BackStencilDepthFailOp)
			|| (currDesc.BackStencilPassOp != stateDesc.BackStencilPassOp))
		{
			glStencilOpSeparate(GL_BACK, OpenGLMapping::Mapping(stateDesc.BackStencilFailOp),
				OpenGLMapping::Mapping(stateDesc.BackStencilDepthFailOp),
				OpenGLMapping::Mapping(stateDesc.BackStencilPassOp) );
		}
	}

	mCurrentDepthStencilState = state;
	mCurrentFrontStencilRef = frontStencilRef;
	mCurrentBackStencilRef = backStencilRef;
}

void OpenGLRenderDevice::DoRender( const EffectTechnique& tech, const RenderOperation& op )
{

	// bind vertex streams
	for (const RenderOperation::StreamUnit& streamUnit : op.VertexStreams)
	{
		// bind vertex buffer
		BindVertexBufferOGL(streamUnit.Stream);

		// set vertex attributes
		const VertexElementList& vertexAttrs = streamUnit.VertexDecl->GetElements(); 
		uint32_t vertexSize = streamUnit.VertexDecl->GetVertexSize();

		for(size_t att = 0; att < vertexAttrs.size(); att++)
		{
			const VertexElement& ve = vertexAttrs[att]; 
			uint16_t count = VertexElement::GetTypeCount(ve.Type);
			GLenum type = OpenGLMapping::Mapping(ve.Type);
			bool isNormalized = VertexElement::IsNormalized(ve.Type);
			uint32_t offset = ve.Offset + op.VertexStart * vertexSize;

			glEnableVertexAttribArray(att);

			if (OpenGLMapping::IsIntegerType(type))
			{
				glVertexAttribIPointer(att, count, type, vertexSize, BUFFER_OFFSET(offset));	
			}
			else
			{
				glVertexAttribPointer(att, count, type, isNormalized, vertexSize, BUFFER_OFFSET(offset));	
			}
		}
	}

	// bind index buffer
	GLenum indexType = GL_UNSIGNED_SHORT;
	uint8_t* indexOffset = NULL;

	if (op.UseIndex)
	{
		if(op.IndexType == IBT_Bit16)
		{
			indexType = GL_UNSIGNED_SHORT;
			indexOffset += op.IndexStart * 2;

		}
		else
		{
			indexType = GL_UNSIGNED_INT;
			indexOffset += op.IndexStart * 4;
		}

		BindIndexBufferOGL(op.IndexBuffer);
	}

	// get pass
	for(EffectPass* pass : tech.GetPasses())
	{
		pass->BeginPass();
	
		if (op.UseIndex)
			glDrawElements(OpenGLMapping::Mapping(op.PrimitiveType), op.IndexCount, indexType, indexOffset);	
		else
			glDrawArrays(OpenGLMapping::Mapping(op.PrimitiveType), op.VertexStart, static_cast<GLsizei>(op.VertexCount));

		pass->EndPass();
	}
}

void OpenGLRenderDevice::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state )
{
	if (mCurrentSamplerStates[unit] != state)
	{
		mCurrentSamplerStates[unit] = state;

		shared_ptr<OpenGLSamplerState> sampler = std::static_pointer_cast<OpenGLSamplerState>(state);
		glBindSampler(unit, sampler->GetSamplerObject());
	}
}

}
