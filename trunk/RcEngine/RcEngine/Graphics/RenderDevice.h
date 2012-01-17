#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "GraphicsCommon.h"
#include "VertexDeclaration.h"
#include "RenderOperation.h"
#include "FrameBuffer.h"
#include "RenderSettings.h"

namespace RcEngine {
namespace RenderSystem {

class _ApiExport RenderDevice
{
public:
	RenderDevice(const RenderSettings& rs);
	virtual ~RenderDevice(void);

	void CreateRenderWindow();

	void Resize();

	FrameBuffer* GetCurrentFrameBuffer();
	void BindFrameBuffer(FrameBuffer* fb);

	virtual void BeginFrame();
	virtual void EndFrame();

	virtual void SwapBuffers();

	virtual void Draw(RenderEffect* effect, const RenderOperation& operation) = 0;

	virtual void BindVertexBuffer(const GraphicBuffer* buffer ) = 0;
	virtual void BindIndexBuffer(const GraphicBuffer* indexBuffer) = 0;

	virtual void AdjustProjectionMatrix(Math::Matrix4f& pOut) = 0;

protected:
	virtual void BindVertexStream(const GraphicBuffer* buffer, const VertexDeclaration& vertexDec) = 0;
	virtual void DoBindFrame(FrameBuffer* fb) = 0;
	
protected:

	unsigned int mWidth, mHeight;

	PixelFormat mColorFormat;
	unsigned int mColorBits;

	unsigned int mDepthBits, mStencilBits;
	bool mIsDepthBuffered;

	FrameBuffer* mCurrentFrameBuffer;
	FrameBuffer* mDefaultFrameBuffer;

	Viewport mViewport;

};

}
}

#endif // RenderDevice_h__