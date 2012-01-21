#ifndef FrameBuffer_h__
#define FrameBuffer_h__

#include "Core/Prerequisites.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Viewport.h"
#include "Math/ColorRGBA.h"

namespace RcEngine {
namespace RenderSystem {

class _ApiExport FrameBuffer
{
public:
	FrameBuffer(unsigned int width, unsigned int height, bool offscreen = true);
	virtual ~FrameBuffer(void);

	bool IsActice() const					{ return mActice; }
	void SetActice(bool state)				{ mActice = state; }

	unsigned int GetLeft() const			{ return mLeft; }
	unsigned int GetTop() const				{ return mTop; }
	unsigned int GetWidth() const			{ return mWidth; }
	unsigned int GetHeight() const			{ return mHeight; }
	
	PixelFormat GetColorFormat() const		{ return mColorFormat; }
	const Viewport&	GetViewport() const		{ return mViewport; }

	unsigned int GetColorDepth() const		{ return mColorDepth; }
	unsigned int GetDepthBits() const		{ return mDepthBits; }
	unsigned int GetStencilBits() const		{ return mStencilBits;}

	bool IsDepthBuffered() const			{ return mIsDepthBuffered; }
	bool IsDirty() const					{ return mDirty; }

	virtual void Clear(unsigned int flags, Math::ColorRGBA& clr, float depth, unsigned int stencil) = 0;
	virtual void SwapBuffers() = 0;

	RenderView* GetAttachedView(unsigned int att);
	 
	void Attach(unsigned int att, RenderView* view);
	void Detach(unsigned int att);

	//Called when this frame buffer is binded
	void OnBind();
	void OnUnbind();

protected:
	virtual void DoBind() = 0;
	virtual void DoUnbind() = 0;

protected:
	bool mActice;

	unsigned int mLeft, mTop;
	unsigned int mWidth, mHeight;

	PixelFormat mColorFormat;
	bool mIsDepthBuffered;
	unsigned int mDepthBits, mStencilBits;
	unsigned int mColorDepth;

	std::vector<RenderView*> mColorViews;
	RenderView* mDepthStencilView;

	Viewport mViewport;

	bool mOffscreen;
	bool mDirty;
};

} // RenderSystem
} // RcEngine

#endif // FrameBuffer_h__
