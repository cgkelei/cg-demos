#ifndef OpenGLRenderView_h__
#define OpenGLRenderView_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/RenderView.h>


namespace RcEngine {

class _OpenGLExport OpenGLRenderView : public RenderView
{
public:
	OpenGLRenderView(void);
	virtual ~OpenGLRenderView(void);

	virtual void ClearColor(const ColorRGBA& clr);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void ClearDepthStencil(float depth, uint32_t stencil);	

	virtual void OnAttach(FrameBuffer& fb, Attachment attr);
	virtual void OnDetach(FrameBuffer& fb, Attachment attr);

protected:
	void DoClear(GLbitfield flag, const ColorRGBA& clr, float depth, uint32_t stencil);

protected:
	Attachment mAttachment;
	GLuint mFrameBufferID;
};


class _OpenGLExport OpenGLDepthStencilView : public OpenGLRenderView
{
public:
	OpenGLDepthStencilView(Texture& texture, uint32_t arrIndex, uint32_t level);
	~OpenGLDepthStencilView(void);

	void ClearDepth(float depth);
	void ClearStencil(uint32_t stencil);
	void ClearDepthStencil(float depth, uint32_t stencil);

	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

private:
	uint32_t mArrIndex;
	uint32_t mLevel;
	OpenGLTexture2D& mTextureOGL;
};

class _OpenGLExport OpenGLScreenDepthStencilView : public OpenGLRenderView
{
public:
	OpenGLScreenDepthStencilView(uint32_t width, uint32_t height, PixelFormat fmt);
	~OpenGLScreenDepthStencilView();

	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

	void ClearDepth(float depth);
	void ClearStencil(uint32_t stencil);
	void ClearDepthStencil(float depth, uint32_t stencil);
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLRenderTargetView2D : public OpenGLRenderView
{
public:
	OpenGLRenderTargetView2D(Texture& texture, uint32_t arrIndex, uint32_t level);
	~OpenGLRenderTargetView2D();

	void ClearColor(const ColorRGBA& clr);

	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);

private:
	uint32_t mArrIndex;
	uint32_t mLevel;
	OpenGLTexture2D& mTextureOGL;
};

class _OpenGLExport OpenGLScreenRenderTargetView2D :  public OpenGLRenderView
{
public:
	OpenGLScreenRenderTargetView2D(uint32_t width, uint32_t height, PixelFormat fmt);
	~OpenGLScreenRenderTargetView2D();
	
	void ClearColor(const ColorRGBA& clr);
	void OnAttach(FrameBuffer& fb, Attachment attr);
	void OnDetach(FrameBuffer& fb, Attachment attr);
};

}
#endif // OpenGLRenderView_h__
