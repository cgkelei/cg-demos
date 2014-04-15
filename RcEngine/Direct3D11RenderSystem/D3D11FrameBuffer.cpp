#include "D3D11FrameBuffer.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11FrameBuffer::D3D11FrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: RHFrameBuffer(width, height, offscreen)
{

}

void D3D11FrameBuffer::SwapBuffers()
{

}

void D3D11FrameBuffer::OnBind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->GetDeviceContextD3D11();


	if (mFrameBufferOGL != 0 && mColorViews.size())
	{
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		std::vector<GLenum> targets(mColorViews.size());
		for (size_t i = 0; i < mColorViews.size(); ++ i)
		{
			if (mColorViews[i])
				targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(static_cast<GLsizei>(targets.size()), &targets[0]);
	}

	deviceContextD3D11->OMSetRenderTargetsAndUnorderedAccessViews()

	OGL_ERROR_CHECK();
}

void D3D11FrameBuffer::OnUnbind()
{

}

}


