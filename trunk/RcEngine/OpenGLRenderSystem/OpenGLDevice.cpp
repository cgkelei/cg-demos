#include "OpenGLDevice.h"
#include "OpenGLFactory.h"
#include "OpenGLBuffer.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderWindow.h"
#include "OpenGLSamplerState.h"
#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLVertexDeclaration.h"
#include <Graphics/RHState.h>
//#include <Graphics/RenderOperation.h>
//#include <Graphics/Material.h>
//#include <Graphics/EffectTechnique.h>
//#include <Graphics/Effect.h>
//#include <Graphics/EffectPass.h>
#include <Graphics/RHOperation.h>
#include <MainApp/Application.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace RcEngine {

OpenGLDevice* gOpenGLDevice = NULL;

OpenGLDevice::OpenGLDevice()
	: mViewportTop(0), 
	  mViewportLeft(0),
	  mViewportWidth(0),
	  mViewportHeight(0),
	  mCurrentFBO(0)
{
	gOpenGLDevice = this;
	mBlitFBO[0] = mBlitFBO[1] = 0; 

	mRHFactory = new OpenGLFactory();
}

OpenGLDevice::~OpenGLDevice(void)
{
	//SAFE_DELETE(mRenderFactory);

	if (mBlitFBO[0] != 0)
		glDeleteFramebuffers(2, mBlitFBO);
}

void OpenGLDevice::CreateRenderWindow()
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	mScreenFrameBuffer = std::make_shared<OpenGLRenderWindow>(appSettings.Width, appSettings.Height);

	mScreenFrameBuffer->AttachRTV(ATT_Color0, std::make_shared<OpenGLScreenRenderTargetView2D>());
	if(PixelFormatUtils::IsDepth(appSettings.DepthStencilFormat))
	{
		// Have depth buffer, attach it
		mScreenFrameBuffer->AttachRTV(ATT_DepthStencil, std::make_shared<OpenGLScreenDepthStencilView>(appSettings.DepthStencilFormat));
	}
	mScreenFrameBuffer->SetViewport(RHViewport(0, 0, appSettings.Width, appSettings.Height));
	
	// Bind as default
	BindFrameBuffer(mScreenFrameBuffer);
	
	//Create default render state
	mCurrentDepthStencilState = mRHFactory->CreateDepthStencilState(RHDepthStencilStateDesc());
	mCurrentBlendState = mRHFactory->CreateBlendState(RHBlendStateDesc());
	mCurrentRasterizerState = mRHFactory->CreateRasterizerState(RHRasterizerStateDesc());

	// enable default render state
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);  // Disable
	glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

	/* glStencil*Separete does't work for back face on my PC.
	glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);*/

	glActiveStencilFaceEXT(GL_FRONT);
	glStencilFunc(GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glActiveStencilFaceEXT(GL_BACK);
	glStencilFunc(GL_ALWAYS, 0, DEFAULT_STENCIL_READ_MASK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// do little hard code
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	OGL_ERROR_CHECK();
}

void OpenGLDevice::GetBlitFBO( GLuint& srcFBO, GLuint& dstFBO )
{
	// Create blit framebuffer
	if (mBlitFBO[0] == 0)
		glGenFramebuffers(2, mBlitFBO);

	srcFBO = mBlitFBO[0];
	dstFBO = mBlitFBO[1];
}

void OpenGLDevice::BindFBO( GLuint fbo )
{
	if (mCurrentFBO != fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		mCurrentFBO = fbo;
	}
}

void OpenGLDevice::AdjustProjectionMatrix( float4x4& pOut )
{
	//修改投影矩阵，使OpenGL适应左右坐标系
	float4x4 scale = CreateScaling(1.0f, 1.0f, 2.0f);
	float4x4 translate = CreateTranslation(0.0f, 0.0f, -1.0f);
	pOut =  pOut * scale * translate;
}

void OpenGLDevice::SetViewport( const RHViewport& vp )
{
	if (vp.Left != mViewportLeft || vp.Top != mViewportTop || vp.Height !=mViewportHeight || vp.Width != mViewportWidth)
	{
		glViewport(vp.Left, vp.Top, vp.Width, vp.Height);
		mViewportLeft = vp.Left;
		mViewportTop = vp.Top;
		mViewportHeight = vp.Height;
		mViewportWidth = vp.Width;
	}

	OGL_ERROR_CHECK();
}


void OpenGLDevice::ToggleFullscreen( bool fs )
{

}


void OpenGLDevice::SetBlendState( const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
{
	OGL_ERROR_CHECK();

	if( mCurrentBlendState != state )
	{
		const RHBlendStateDesc& currDesc = mCurrentBlendState->GetDesc();
		const RHBlendStateDesc& stateDesc = state->GetDesc();

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

				if (currDesc.RenderTarget[i].BlendOp != stateDesc.RenderTarget[i].BlendOp)
				{
					glBlendEquationSeparatei(i, OpenGLMapping::Mapping(stateDesc.RenderTarget[i].BlendOp),
											    OpenGLMapping::Mapping(stateDesc.RenderTarget[i].BlendOpAlpha));
				}

				if ( (currDesc.RenderTarget[i].SrcBlend != stateDesc.RenderTarget[i].SrcBlend) || 
					 (currDesc.RenderTarget[i].DestBlend != stateDesc.RenderTarget[i].DestBlend)|| 
					 (currDesc.RenderTarget[i].SrcBlendAlpha != stateDesc.RenderTarget[i].SrcBlendAlpha) || 
					 (currDesc.RenderTarget[i].DestBlendAlpha != stateDesc.RenderTarget[i].DestBlendAlpha) )
				{
					glBlendFuncSeparatei(i, OpenGLMapping::Mapping(stateDesc.RenderTarget[i].SrcBlend), 
									        OpenGLMapping::Mapping(stateDesc.RenderTarget[i].DestBlend),
									        OpenGLMapping::Mapping(stateDesc.RenderTarget[i].SrcBlendAlpha), 
										    OpenGLMapping::Mapping(stateDesc.RenderTarget[i].DestBlendAlpha));
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

			if (currDesc.RenderTarget[0].BlendOp != stateDesc.RenderTarget[0].BlendOp)
			{
				glBlendEquationSeparate(OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOp),
					OpenGLMapping::Mapping(stateDesc.RenderTarget[0].BlendOpAlpha));
			}

			if ( (currDesc.RenderTarget[0].SrcBlend != stateDesc.RenderTarget[0].SrcBlend) || 
				 (currDesc.RenderTarget[0].DestBlend != stateDesc.RenderTarget[0].DestBlend) || 
				 (currDesc.RenderTarget[0].SrcBlendAlpha != stateDesc.RenderTarget[0].SrcBlendAlpha) || 
				 (currDesc.RenderTarget[0].DestBlendAlpha != stateDesc.RenderTarget[0].DestBlendAlpha) )
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

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetRasterizerState( const shared_ptr<RHRasterizerState>& state )
{
	OGL_ERROR_CHECK();

	if(mCurrentRasterizerState != state)
	{
		const RHRasterizerStateDesc& currDesc = mCurrentRasterizerState->GetDesc();
		const RHRasterizerStateDesc& stateDesc = state->GetDesc();

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

		if (currDesc.DepthBias != stateDesc.DepthBias || currDesc.SlopeScaledDepthBias != stateDesc.SlopeScaledDepthBias)
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

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetDepthStencilState( const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef, uint16_t backStencilRef )
{
	OGL_ERROR_CHECK();

	if ( mCurrentDepthStencilState != state || mCurrentFrontStencilRef != frontStencilRef || mCurrentBackStencilRef != backStencilRef )
	{
		const RHDepthStencilStateDesc& currDesc = mCurrentDepthStencilState->GetDesc();
		const RHDepthStencilStateDesc& stateDesc = state->GetDesc();

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

		if (currDesc.FrontStencilFunc != stateDesc.FrontStencilFunc || 
			currDesc.StencilReadMask != stateDesc.StencilReadMask   || 
			mCurrentFrontStencilRef != frontStencilRef)
		{
			glActiveStencilFaceEXT(GL_FRONT);
			glStencilFunc(OpenGLMapping::Mapping(stateDesc.FrontStencilFunc), frontStencilRef, stateDesc.StencilReadMask);
		}

		if (currDesc.FrontStencilFailOp != stateDesc.FrontStencilFailOp || 
			currDesc.FrontStencilDepthFailOp != stateDesc.FrontStencilDepthFailOp || 
			currDesc.FrontStencilPassOp != stateDesc.FrontStencilPassOp)
		{
			glActiveStencilFaceEXT(GL_FRONT);
			glStencilOp(OpenGLMapping::Mapping(stateDesc.FrontStencilFailOp),
					    OpenGLMapping::Mapping(stateDesc.FrontStencilDepthFailOp),
						OpenGLMapping::Mapping(stateDesc.FrontStencilPassOp));
		}

		if (currDesc.BackStencilFunc != stateDesc.BackStencilFunc ||
			currDesc.StencilReadMask != stateDesc.StencilReadMask ||
			mCurrentBackStencilRef != backStencilRef)
		{
			glActiveStencilFaceEXT(GL_BACK);
			glStencilFunc(OpenGLMapping::Mapping(stateDesc.BackStencilFunc), backStencilRef, stateDesc.StencilReadMask);
		}

		if (currDesc.BackStencilFailOp != stateDesc.BackStencilFailOp ||
			currDesc.BackStencilDepthFailOp != stateDesc.BackStencilDepthFailOp ||
			currDesc.BackStencilPassOp != stateDesc.BackStencilPassOp)
		{
			glActiveStencilFaceEXT(GL_BACK);
			glStencilOp(OpenGLMapping::Mapping(stateDesc.BackStencilFailOp),
				        OpenGLMapping::Mapping(stateDesc.BackStencilDepthFailOp),
						OpenGLMapping::Mapping(stateDesc.BackStencilPassOp));
		}
	}

	mCurrentDepthStencilState = state;
	mCurrentFrontStencilRef = frontStencilRef;
	mCurrentBackStencilRef = backStencilRef;

	OGL_ERROR_CHECK();
}

void OpenGLDevice::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state )
{
	if (mCurrentSamplerStates[unit] != state)
	{
		mCurrentSamplerStates[unit] = state;

		OpenGLSamplerState* pSamplerState = static_cast_checked<OpenGLSamplerState*>(state.get());
		glBindSampler(unit, pSamplerState->GetSamplerOGL());

		OGL_ERROR_CHECK();
	}
}

void OpenGLDevice::SetupVertexArray( const RHOperation& operation )
{
	OpenGLVertexDeclaration* vertexDeclOGL = static_cast_checked<OpenGLVertexDeclaration*>(operation.mVertexDecl.get());

	GLuint vertexArrayOGL;
	bool bCreated = vertexDeclOGL->GetOrGenVertexArrayOGL(vertexArrayOGL);
	glBindVertexArray(vertexArrayOGL);

	if (!bCreated)
	{
		std::vector<bool> slotBind(operation.mVertexStreams.size(), false);
		for (GLuint attribIndex = 0; attribIndex < vertexDeclOGL->mVertexElemets.size(); ++attribIndex)
		{
			const RHVertexElement& attribute = vertexDeclOGL->mVertexElemets[attribIndex];

			OpenGLBuffer* bufferOGL = static_cast_checked<OpenGLBuffer*>(operation.mVertexStreams[attribute.InputSlot].get());
			glBindBuffer(GL_ARRAY_BUFFER, bufferOGL->GetBufferOGL());


			/*uint16_t count = VertexElement::GetTypeCount(ve.Type);
			GLenum type = OpenGLMapping::Mapping(ve.Type);
			bool isNormalized = VertexElement::IsNormalized(ve.Type);
			uint32_t offset = ve.Offset + op.VertexStart * vertexSize;*/

			uint16_t count;
			GLenum type;
			bool isNormalized;
			uint32_t offset;
			uint32_t vertexSize;
			
			bool isMatchVertexShader = false;

			if (isMatchVertexShader)
			{
				glEnableVertexAttribArray(attribIndex);

				if (OpenGLMapping::IsIntegerType(type))
					glVertexAttribIPointer(attribIndex, count, type, vertexSize, BUFFER_OFFSET(offset));	
				else
					glVertexAttribPointer(attribIndex, count, type, isNormalized, vertexSize, BUFFER_OFFSET(offset));	

				if (attribute.InstanceStepRate > 0)
					glVertexAttribDivisor(attribIndex, attribute.InstanceStepRate);
			}
			else 
				glDisableVertexAttribArray(attribIndex);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void OpenGLDevice::DoDraw(const RHOperation& operation)
{
	// Commit all shader parameter
	glBindProgramPipeline(mProgramPipeline);
	
	SetupVertexArray(operation);

	GLenum primitiveTypeOGL = OpenGLMapping::Mapping(operation.mPrimitiveType);
	if (operation.mIndexBuffer)
	{
		SetupIndexBuffer(operation);

		GLenum indexTypeOGL = GL_UNSIGNED_SHORT;
		uint8_t* indexOffset = NULL;
		if(operation.IndexType == IBT_Bit16)
		{
			indexTypeOGL = GL_UNSIGNED_SHORT;
			indexOffset += operation.mIndexStart * 2;
		}
		else
		{
			indexTypeOGL = GL_UNSIGNED_INT;
			indexOffset += operation.mIndexStart * 4;
		}
	
		//foreach pass 
		{
			if (operation.mNumInstances <= 1)
				glDrawElements(primitiveTypeOGL, operation.mIndexCount, indexTypeOGL, indexOffset);
			else
				glDrawElementsInstanced(primitiveTypeOGL, operation.mIndexCount, indexTypeOGL, indexOffset, operation.mNumInstances);
		}
		
	}
	else
	{
		//foreach pass 
		{
			if (operation.mNumInstances <= 1)
				glDrawArrays(primitiveTypeOGL, operation.mVertexStart, operation.mVertexCount);
			else
				glDrawArraysInstanced(primitiveTypeOGL, operation.mVertexStart, operation.mVertexCount, operation.mNumInstances);
		}
	}
}

void OpenGLDevice::SetupIndexBuffer( const RHOperation& operation )
{

}


}
