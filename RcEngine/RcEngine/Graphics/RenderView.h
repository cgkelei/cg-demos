#ifndef RenderView_h__
#define RenderView_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {
namespace Render {

class _ApiExport RenderView
{
public: 
	RenderView();
	virtual ~RenderView();

	uint32_t GetWidth() const  { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	PixelFormat GetFormat() const  { return mFormat; }

	virtual void ClearColor(const Math::ColorRGBA& clr) = 0;
	virtual void ClearDepth(float depth) = 0;
	virtual void ClearDepthStencil(float depth, uint32_t stencil) = 0;

	virtual void OnAttach(FrameBuffer* fb, uint32_t attr) = 0;
	virtual void OnDetach(FrameBuffer* fb, uint32_t attr) = 0;

	virtual void OnBind(FrameBuffer* fb, uint32_t att);
	virtual void OnUnbind(FrameBuffer* fb, uint32_t att);

protected:
	uint32_t mWidth, mHeight;
	PixelFormat mFormat;
	FrameBuffer* mAttachedFrameBuffer;

};


} // Namespace Render
} // Namespace RcEngine



#endif // RenderView_h__
