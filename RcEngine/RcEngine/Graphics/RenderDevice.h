#ifndef RenderDevice_h__
#define RenderDevice_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderSettings.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport RenderDevice
{
public:
	RenderDevice(void);
	virtual ~RenderDevice(void);

	RenderFactory* GetRenderFactory() const;

	FrameBuffer* GetCurrentFrameBuffer() const	{ return mCurrentFrameBuffer; }
	FrameBuffer* GetScreenFrameBuffer() const	{ return mScreenFrameBuffer; }
			
	void BindFrameBuffer(FrameBuffer* fb);

	void Render( EffectTechnique& tech, RenderOperation& op);
	void Resize(uint32_t width, uint32_t height);

	virtual void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	/*virtual void SetSamplerState(const shared_ptr<SamplerState>& state);*/
	virtual void SetRasterizerState(const shared_ptr<RasterizerState>& state) = 0;
	virtual void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef, uint16_t backStencilRef) = 0;

	virtual void Create() = 0;
	virtual void Release() = 0;
	virtual void ToggleFullscreen(bool fs) = 0;
	virtual bool Fullscreen() const = 0;
	virtual void CreateRenderWindow(const RenderSettings& settings) = 0;	
	virtual void AdjustProjectionMatrix(Matrix4f& pOut) = 0;

protected:
	virtual void DoBindFrameBuffer(FrameBuffer* fb) = 0;
	virtual void DoRender( EffectTechnique& tech, RenderOperation& op) = 0;

protected:
	uint32_t mWidth, mHeight;
	PixelFormat mColorFormat;
	uint32_t mColorBits;

	uint32_t mDepthBits, mStencilBits;
	bool mIsDepthBuffered;

	FrameBuffer* mCurrentFrameBuffer;
	FrameBuffer* mScreenFrameBuffer;
			
	RenderSettings mRenderSettings;

	RenderFactory* mRenderFactory;

	shared_ptr<BlendState> mCurrentBlendState;
	shared_ptr<SamplerState> mCurrentSamplerState;
	shared_ptr<RasterizerState> mCurrentRasterizerState;
	shared_ptr<DepthStencilState> mCurrentDepthStencilState;

	ColorRGBA mCurrentBlendFactor;
	uint32_t mCurrentSampleMask;

	uint16_t mCurrentFrontStencilRef, mCurrentBackStencilRef;
		
};

} // Namespace RcEngine

#endif // RenderDevice_h__