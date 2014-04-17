#ifndef RHFrameBuffer_h__
#define RHFrameBuffer_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

/**
 * No Multiple Viewports support.
 */
struct _ApiExport RHViewport
{
	RHViewport() {}
	RHViewport(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
		: Left(left), Top(top), Width(width), Height(height) {}
	
	friend bool operator != ( const RHViewport& lhs, const RHViewport& rhs )
	{
		return (lhs.Left != rhs.Left) || (lhs.Top != rhs.Top) || 
			(lhs.Width != rhs.Width) || (lhs.Height != rhs.Height);
	}

	uint32_t Left, Top, Width, Height;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHRenderView
{
public:
	friend class RHFrameBuffer;

public: 
	RHRenderView(const shared_ptr<RHTexture>& texture);
	virtual ~RHRenderView() {}
	
	virtual void ClearColor(const ColorRGBA& clr) = 0;
	virtual void ClearDepth(float depth) = 0;
	virtual void ClearStencil(uint32_t stencil) = 0;
	virtual void ClearDepthStencil(float depth, uint32_t stencil) = 0;

protected:
	/**
	 * Called when a render view is attached to a frame buffer.
	 * Only called by frame buffer class;
	 */
	virtual void OnAttach(RHFrameBuffer& fb, Attachment attr) = 0;
	virtual void OnDetach(RHFrameBuffer& fb, Attachment attr) = 0;

protected:
	shared_ptr<RHTexture> mTexture;  // Keep a reference of texture, keep it alive, never use it
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHFrameBuffer
{
public:
	RHFrameBuffer(bool offscreen = true);
	virtual ~RHFrameBuffer();

	inline bool IsDirty() const			{ return mDirty; }

	void SetViewport(const RHViewport& vp);
	inline const RHViewport& GetViewport() const { return mViewport; }

	shared_ptr<RHRenderView> GetRTV(Attachment att) const;
	shared_ptr<RHUnorderedAccessView> GetUAV(uint32_t index) const;

	void AttachRTV(Attachment att, const shared_ptr<RHRenderView>& rtv);
	void AttachUAV(uint32_t index, const shared_ptr<RHUnorderedAccessView>& uav);

	void DetachRTV(Attachment att);
	void DetachUAV(uint32_t index);

	void DetachAll();

	/**
	 * Clear all render target of the frame buffer.
	 * note that before do clear, you need to bind the frame buffer as current device 
	 * frame buffer first, it is user's responsibility.
	 */
	void Clear(uint32_t flags, const ColorRGBA& clr, float depth, uint32_t stencil);

	virtual void SwapBuffers() = 0;

	//Called when this frame buffer is binded
	virtual void OnBind() = 0;
	virtual void OnUnbind() = 0;

protected:

	// Todo: add multiple viewports support
	RHViewport mViewport;

	vector<shared_ptr<RHRenderView> > mColorViews;
	shared_ptr<RHRenderView> mDepthStencilView;

	// Unordered access view, used in per-pixel list method, like OIT.
	vector<shared_ptr<RHUnorderedAccessView>> mUnorderedAccessViews;
	
	bool mOffscreen;
	bool mDirty;

	//shared_ptr<Camera> mCamera;
};

}

#endif // RHFrameBuffer_h__
