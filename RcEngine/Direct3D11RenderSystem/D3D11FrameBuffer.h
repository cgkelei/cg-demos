#ifndef D3D11FrameBuffer_h__
#define D3D11FrameBuffer_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHFrameBuffer.h>

namespace RcEngine {

class _D3D11Export D3D11FrameBuffer : public RHFrameBuffer
{
public:
	D3D11FrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);

	virtual void SwapBuffers();
	virtual void OnBind();
	virtual void OnUnbind();

protected:


};


}


#endif // D3D11FrameBuffer_h__