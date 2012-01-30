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

	}
}
