#include "OpenGLRenderDevice.h"
#include "OpenGLRenderFactory.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLRenderEffect.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderWindow.h"
#include "OpenGLRenderView.h"
#include "OpenGLTexture.h"
#include "Graphics/Viewport.h"
#include "Core/Exception.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace RcEngine
{
	namespace Render
	{

		OpenGLRenderDevice::OpenGLRenderDevice()
		{
	
		}


		OpenGLRenderDevice::~OpenGLRenderDevice(void)
		{
		}

		void OpenGLRenderDevice::Create()
		{
			mRenderFactory = new OpenGLRenderFactory();

		}

		void OpenGLRenderDevice::Release()
		{
			Safe_Delete(mRenderFactory);
		}

		void OpenGLRenderDevice::InitGlew()
		{
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				String errMsg = reinterpret_cast<char const *>(glewGetErrorString(err));
				ENGINE_EXCEPT(Core::Exception::ERR_RENDERINGAPI_ERROR, errMsg, "OpenGLRenderDevice::InitGlew");
			}
		}

		void OpenGLRenderDevice::CreateRenderWindow( const RenderSettings& settings )
		{
			mRenderSettings = settings;

			mDefaultFrameBuffer = new OpenGLRenderWindow(settings);

			// create a valid OpenGL rendering context and init glew
			InitGlew();

			mDefaultFrameBuffer->Attach(ATT_Color0, 
				new OpenGLScreenRenderTarget2DView(mDefaultFrameBuffer->GetWidth(), mDefaultFrameBuffer->GetHeight(), mDefaultFrameBuffer->GetColorFormat()));
			
			if(mDefaultFrameBuffer->IsDepthBuffered())
			{
				mDefaultFrameBuffer->Attach(ATT_DepthStencil,
					new OpenGLScreenDepthStencilView(mDefaultFrameBuffer->GetWidth(), mDefaultFrameBuffer->GetHeight(), settings.DepthStencilFormat));
			}
		
			this->BindFrameBuffer(mDefaultFrameBuffer);
		}

		void OpenGLRenderDevice::AdjustProjectionMatrix( Math::Matrix4f& pOut )
		{
			//修改投影矩阵，使OpenGL适应左右坐标系
			Math::Matrix4f scale = Math::MathUtil::CreateScaling(1.0f, 1.0f, 2.0f);
			Math::Matrix4f translate = Math::MathUtil::CreateTranslation(0.0f, 0.0f, -1.0f);
			pOut =  pOut * scale * translate;
		}

		void OpenGLRenderDevice::BindVertexBufferOGL( const GraphicsBuffer* buffer )
		{
			const OpenGLGraphicsBuffer* pBuffer = (const OpenGLGraphicsBuffer*)buffer;
			glBindBuffer(GL_ARRAY_BUFFER, pBuffer->GetBufferID());
		}

		void OpenGLRenderDevice::BindIndexBufferOGL( const GraphicsBuffer* indexBuffer )
		{
			const OpenGLGraphicsBuffer* pBuffer =  (const OpenGLGraphicsBuffer*)(indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->GetBufferID());
		}

		void OpenGLRenderDevice::BindOutputStreams( RenderOperation& operation )
		{	
			for (uint32 i = 0; i < operation.GetStreamCount(); i++)
			{
				RenderOperation::StreamUnit streamUnit = operation.GetStreamUnit(i);

				BindVertexBufferOGL(streamUnit.Stream);

				const VertexElementList& vertexAttrs = streamUnit.VertexDecl->GetElements(); 
				uint32 vertexSize = streamUnit.VertexSize;

				for(size_t att = 0; att < vertexAttrs.size(); att++)
				{
					const VertexElement& ve = vertexAttrs[att]; 
					uint16 count = VertexElement::GetTypeCount(ve.GetType());
					GLenum type = OpenGLMapping::Mapping(ve.GetType());
					bool isNormalized = VertexElement::IsNormalized(ve.GetType());
					uint32 offset = ve.GetOffset() + operation.GetBaseVertexLocation() * vertexSize;

					assert(type = GL_FLOAT);
					glVertexAttribPointer(att, count, type, isNormalized, vertexSize, BUFFER_OFFSET(offset));
					glEnableVertexAttribArray(att);
				}
			}

			if(operation.UseIndices())
			{
				BindIndexBufferOGL(operation.GetIndexStream());
			}
		}

		void OpenGLRenderDevice::Draw( RenderTechnique& tech, RenderOperation& operation )
		{

			GLenum indexType = GL_UNSIGNED_SHORT;
			uint8* indexOffset = NULL;

			GLenum mode = OpenGLMapping::Mapping(operation.GetPrimitiveType());
			RenderTechnique::PassList Passes = tech.GetPasses();

			BindOutputStreams(operation);

			if(operation.UseIndices())
			{
				if(operation.GetIndexType() == IBT_Bit16)
				{
					indexType = GL_UNSIGNED_SHORT;
					indexOffset += operation.GetStartIndexLocation() * 2;

				}
				else
				{
					indexType = GL_UNSIGNED_INT;
					indexOffset += operation.GetStartIndexLocation() * 4;
				}

				for(RenderTechnique::PassList::iterator pass = Passes.begin(); pass != Passes.end(); ++pass)
				{
					if ( (*pass)->BeginPass() )
					{
						glDrawElements(GL_TRIANGLES, operation.GetIndicesCount(), indexType, indexOffset);	
						(*pass)->EndPass();
					}
				}

			}
			else
			{
				for(RenderTechnique::PassList::iterator pass = Passes.begin(); pass != Passes.end(); ++pass)
				{
					if ( (*pass)->BeginPass() )
					{
						glDrawArrays(mode, operation.GetStartVertexLocation(), static_cast<GLsizei>(operation.GetVertexCount()));

						(*pass)->EndPass();
					}
				}
				
			}
		}

		void OpenGLRenderDevice::DoBindFrameBuffer( FrameBuffer* fb )
		{
			OpenGLFrameBuffer* oglFrameBuffer = static_cast<OpenGLFrameBuffer*>(fb);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oglFrameBuffer->GetFrameBufferObject());

			const Viewport& vp = fb->GetViewport();
			if (!(mViewport == vp))
			{
				mViewport = vp;
				glViewport(vp.mLeft, vp.mTop, vp.mWidth, vp.mHeight);
			}
		}

		void OpenGLRenderDevice::ToggleFullscreen( bool fs )
		{

		}

		bool OpenGLRenderDevice::Fullscreen() const
		{
			return false;
		}

		void OpenGLRenderDevice::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32 sampleMask )
		{
			if( mCurrentBlendState != state )
			{
				const BlendStateDesc& currDesc = mCurrentBlendState->GetDesc();
				const BlendStateDesc& stateDesc = state->GetDesc();

				if (currDesc.AlphaToCoverageEnable != stateDesc.AlphaToCoverageEnable)
				{
					if (stateDesc.AlphaToCoverageEnable)
					{
						glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
					}
					else
					{
						glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
					}
				}

				if( GLEW_EXT_draw_buffers2 ) 
				{
					for(int i = 0; i < 8; i++)
					{
						if(currDesc.RenderTarget[i].BlendEnable != stateDesc.RenderTarget[i].BlendEnable)
						{
							if (stateDesc.RenderTarget[i].BlendEnable)
							{
								glEnableIndexedEXT(GL_BLEND, i);
							}
							else
							{
								glDisableIndexedEXT(GL_BLEND, i);
							}
						}

						if (currDesc.RenderTarget[i].RenderTargetWriteMask != stateDesc.RenderTarget[i].RenderTargetWriteMask)
						{
							glColorMaskIndexedEXT(i, (stateDesc.RenderTarget[i].RenderTargetWriteMask & CWM_Red) != 0,
								(stateDesc.RenderTarget[i].RenderTargetWriteMask & CWM_Green) != 0,
								(stateDesc.RenderTarget[i].RenderTargetWriteMask & CWM_Blue) != 0,
								(stateDesc.RenderTarget[i].RenderTargetWriteMask & CWM_Alpha) != 0 );
						}

					}
				}
				else
				{
					if (currDesc.RenderTarget[0].BlendEnable != stateDesc.RenderTarget[0].BlendEnable)
					{
						if (stateDesc.RenderTarget[0].BlendEnable)
						{
							glEnable(GL_BLEND);
						}
						else
						{
							glDisable(GL_BLEND);
						}
					}

					if (currDesc.RenderTarget[0].RenderTargetWriteMask != stateDesc.RenderTarget[0].RenderTargetWriteMask)
					{
						glColorMask( (stateDesc.RenderTarget[0].RenderTargetWriteMask & CWM_Red) != 0,
							(stateDesc.RenderTarget[0].RenderTargetWriteMask & CWM_Green) != 0,
							(stateDesc.RenderTarget[0].RenderTargetWriteMask & CWM_Blue) != 0,
							(stateDesc.RenderTarget[0].RenderTargetWriteMask & CWM_Alpha) != 0 );
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

		void OpenGLRenderDevice::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16 frontStencilRef, uint16 backStencilRef )
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
					glStencilMask(stateDesc.DepthWriteMask);
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

	}
}
