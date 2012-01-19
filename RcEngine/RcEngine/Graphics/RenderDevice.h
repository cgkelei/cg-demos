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
	RenderDevice(void);
	virtual ~RenderDevice(void);

	RenderFactory* GetRenderFactory() const;
	
	void BindFrameBuffer(FrameBuffer* fb);
	FrameBuffer* GetCurrentFrameBuffer();

	virtual void Create() = 0;
	virtual void Release() = 0;

	


	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void ToggleFullscreen(bool fs) = 0;
	virtual bool IsFullscreen() const = 0;

	virtual void CreateRenderWindow(const RenderSettings& settings) = 0;
	virtual void BindVertexBuffer(const GraphicBuffer* vertexBuffer ) = 0;
	virtual void BindIndexBuffer(const GraphicBuffer* indexBuffer) = 0;
	virtual void Draw(RenderEffect* effect, const RenderOperation& operation) = 0;
	virtual void AdjustProjectionMatrix(Math::Matrix4f& pOut) = 0;

	void Resize(unsigned int width, unsigned int height);

	

	

	virtual void SwapBuffers();


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

	RenderSettings mRenderSettings;

	Viewport mViewport;

	RenderFactory* mRenderFactory;
};

}
}

#endif // RenderDevice_h__