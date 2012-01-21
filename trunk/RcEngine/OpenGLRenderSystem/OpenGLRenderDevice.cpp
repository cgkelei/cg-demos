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
	namespace RenderSystem
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

		void OpenGLRenderDevice::Draw( RenderEffect* effect, const RenderOperation& operation )
		{
			RenderTechnique* technique = effect->GetTechniques()[0];
			RenderTechnique::PassList Passes = technique->GetPasses();
			/*BindVertexStream(operation.mVertexBuffer, operation.mVertexDecl);
			if(operation.mUseIndex)
			{
				BindIndexBuffer(operation.mIndexBuffer);
			}*/
	
			for(RenderTechnique::PassList::iterator pass = Passes.begin(); pass != Passes.end(); ++pass)
			{        
				if ( (*pass)->BeginPass() )
				{
					glBegin(GL_QUADS);
					glVertex3f(-1, 1, 0);
					glVertex3f(-1, -1, 0);
					glVertex3f(1, -1, 0);
					glVertex3f(1, 1, 0);
					glEnd();

					//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)0);
					(*pass)->EndPass();
				}
			}
		}

		void OpenGLRenderDevice::AdjustProjectionMatrix( Math::Matrix4f& pOut )
		{
			//修改投影矩阵，使OpenGL适应左右坐标系
			Math::Matrix4f scale = Math::MathUtil::CreateScaling(1.0f, 1.0f, 2.0f);
			Math::Matrix4f translate = Math::MathUtil::CreateTranslation(0.0f, 0.0f, -1.0f);
			pOut =  pOut * scale * translate;
		}

		void OpenGLRenderDevice::BindVertexBuffer( const GraphicBuffer* buffer )
		{
			const OpenGLGraphicBuffer* pBuffer = (const OpenGLGraphicBuffer*)buffer;
			glBindBuffer(GL_ARRAY_BUFFER, pBuffer->GetBufferID());
		}

		void OpenGLRenderDevice::BindIndexBuffer( const GraphicBuffer* indexBuffer )
		{
			const OpenGLGraphicBuffer* pBuffer =  (const OpenGLGraphicBuffer*)(indexBuffer);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->GetBufferID());
		}

		void OpenGLRenderDevice::BindVertexStream( const GraphicBuffer* buffer, const VertexDeclaration& vertexDec )
		{
			BindVertexBuffer(buffer);
			const VertexElementList& vertexAttrs = vertexDec.GetElements(); 
			uint32 vertexSize = vertexDec.GetVertexSize();
			for(size_t i = 0; i < vertexAttrs.size(); i++)
			{
				const VertexElement& ve = vertexAttrs[i]; 
				uint16 count = VertexElement::GetTypeCount(ve.GetType());
				GLenum type = OpenGLMapping::Mapping(ve.GetType());
				bool isNormalized = VertexElement::IsNormalized(ve.GetType());
				uint32 offset = ve.GetOffset();

				assert (type == GL_FLOAT);
				glVertexAttribPointer(i, count, type, isNormalized, vertexSize, BUFFER_OFFSET(offset));
				glEnableVertexAttribArray(i);
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

		void OpenGLRenderDevice::InitGlew()
		{
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				String errMsg = reinterpret_cast<char const *>(glewGetErrorString(err));
				ENGINE_EXCEPT(Core::Exception::ERR_RENDERINGAPI_ERROR, errMsg, "OpenGLRenderDevice::InitGlew");
			}
		}
	}
}
