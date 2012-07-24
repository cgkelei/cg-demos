#ifndef FrameBuffer_h__
#define FrameBuffer_h__

#include "Core/Prerequisites.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Viewport.h"
#include "Math/ColorRGBA.h"

namespace RcEngine {
namespace Render {

class _ApiExport FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
	virtual ~FrameBuffer(void);

	bool IsActice() const					{ return mActice; }
	void SetActice(bool state)				{ mActice = state; }

	int32_t GetLeft() const			    { return mLeft; }
	int32_t GetTop() const				{ return mTop; }
	uint32_t GetWidth() const			{ return mWidth; }
	uint32_t GetHeight() const			{ return mHeight; }
	
	PixelFormat GetColorFormat() const		{ return mColorFormat; }
	const Viewport&	GetViewport() const		{ return mViewport; }

	uint32_t GetColorDepth() const		{ return mColorDepth; }
	uint32_t GetDepthBits() const		{ return mDepthBits; }
	uint32_t GetStencilBits() const		{ return mStencilBits;}

	bool IsDepthBuffered() const			{ return mIsDepthBuffered; }
	bool IsDirty() const					{ return mDirty; }

	virtual void Clear(uint32_t flags, Math::ColorRGBA& clr, float depth, uint32_t stencil) = 0;
	virtual void SwapBuffers() = 0;

	Camera* GetCamera() const			{ return mViewport.AttachCamera; }

	RenderView* GetAttachedView(uint32_t att);
	 
	void Attach(uint32_t att, RenderView* view);
	void Detach(uint32_t att);

	//Called when this frame buffer is binded
	void OnBind();
	void OnUnbind();

protected:
	virtual void DoBind() = 0;
	virtual void DoUnbind() = 0;

protected:
	bool mActice;

	int32_t mLeft, mTop;
	uint32_t mWidth, mHeight;

	PixelFormat mColorFormat;
	bool mIsDepthBuffered;
	uint32_t mDepthBits, mStencilBits;
	uint32_t mColorDepth;

	std::vector<RenderView*> mColorViews;
	RenderView* mDepthStencilView;

	Viewport mViewport;

	bool mOffscreen;
	bool mDirty;
};

} // RenderSystem
} // RcEngine

#endif // FrameBuffer_h__
