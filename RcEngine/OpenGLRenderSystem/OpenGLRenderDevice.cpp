#include "OpenGLRenderDevice.h"
#include "OpenGLGraphicBuffer.h"
#include "OpenGLRenderEffect.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include "Graphics/Viewport.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace RcEngine
{
	namespace RenderSystem
	{

		OpenGLRenderDevice::OpenGLRenderDevice( const RenderSettings& rs )
			: RenderDevice(rs)
		{

		}


		OpenGLRenderDevice::~OpenGLRenderDevice(void)
		{
		}

		void OpenGLRenderDevice::Draw( RenderEffect* effect, const RenderOperation& operation )
		{
			RenderTechnique* technique = effect->GetTechniques()[0];
			RenderTechnique::PassList Passes = technique->GetPasses();
			BindVertexStream(operation.mVertexBuffer, operation.mVertexDecl);
			if(operation.mUseIndex)
			{
				BindIndexBuffer(operation.mIndexBuffer);
			}
	
			for(RenderTechnique::PassList::iterator pass = Passes.begin(); pass != Passes.end(); ++pass)
			{        
				if ( (*pass)->BeginPass() )
				{
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)0);
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

		void OpenGLRenderDevice::DoBindFrame( FrameBuffer* fb )
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

	}
}
