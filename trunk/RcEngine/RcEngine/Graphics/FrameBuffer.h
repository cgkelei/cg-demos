#ifndef FrameBuffer_h__
#define FrameBuffer_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Viewport.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {


class _ApiExport FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
	virtual ~FrameBuffer(void);

	bool IsActice() const					{ return mActice; }
	void SetActice(bool state)				{ mActice = state; }

	uint32_t GetWidth() const			{ return mWidth; }
	uint32_t GetHeight() const			{ return mHeight; }
	
	PixelFormat GetColorFormat() const		{ return mColorFormat; }
	const Viewport&	GetViewport() const		{ return mViewport; }

	uint32_t GetColorDepth() const		{ return mColorDepth; }
	uint32_t GetDepthBits() const		{ return mDepthBits; }
	uint32_t GetStencilBits() const		{ return mStencilBits;}

	bool IsDepthBuffered() const			{ return mIsDepthBuffered; }
	bool IsDirty() const					{ return mDirty; }

	/**
	 * Clear all render target of the frame buffer.
	 * note that before do clear, you need to bind the frame buffer as current device 
	 * frame buffer first, it is user's responsibity.
	 */
	virtual void Clear(uint32_t flags, ColorRGBA& clr, float depth, uint32_t stencil) = 0;
	
	
	virtual void SwapBuffers() = 0;
	
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	Camera* GetCamera() const			{ return mViewport.AttachCamera; }

	shared_ptr<RenderView> GetAttachedView(Attachment att);
	 
	void Attach(Attachment att, const shared_ptr<RenderView>& view);
	void Detach(Attachment att);

	//Called when this frame buffer is binded
	void OnBind();
	void OnUnbind();

protected:
	virtual void DoBind() = 0;
	virtual void DoUnbind() = 0;

protected:
	bool mActice;

	uint32_t mWidth, mHeight;

	PixelFormat mColorFormat;
	bool mIsDepthBuffered;
	uint32_t mDepthBits, mStencilBits;
	uint32_t mColorDepth;

	vector<shared_ptr<RenderView> > mColorViews;
	shared_ptr<RenderView> mDepthStencilView;

	Viewport mViewport;

	bool mOffscreen;
	bool mDirty;
};


} // Namespace RcEngine

#endif // FrameBuffer_h__
