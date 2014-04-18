#ifndef RHDevice_h__
#define RHDevice_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>

#define MaxSamplerCout 16

namespace RcEngine {

class RHFactory;
struct RHViewport;

class _ApiExport RHDevice
{
public:
	RHDevice();
	virtual ~RHDevice(void);

	inline bool Fullscreen() const												    { return mIsFullscreen; }
	inline RHFactory* GetFactory() const									        { return mRHFactory; }
																  
	inline shared_ptr<RHFrameBuffer> GetCurrentFrameBuffer() const					{ return mCurrentFrameBuffer; }
	inline shared_ptr<RHFrameBuffer> GetScreenFrameBuffer() const					{ return mScreenFrameBuffer; }
	inline shared_ptr<RHDepthStencilState> GetCurrentDepthStencilState() const	    { return mCurrentDepthStencilState; }
	inline shared_ptr<RHBlendState> GetCurrentBlendState() const					{ return mCurrentBlendState; }

	virtual void ToggleFullscreen(bool fs) = 0;
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

	virtual void SetRasterizerState(const shared_ptr<RHRasterizerState>& state) = 0;
	virtual void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state) = 0;
	virtual void SetBlendState(const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	virtual void SetDepthStencilState(const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0) = 0;
	
	void BindFrameBuffer(const shared_ptr<RHFrameBuffer>& fb);
	void BindShaderPipeline(const shared_ptr<RHShaderPipeline>& pipeline);
	void Draw(const RHOperation& operation);

protected:
	virtual void DoDraw(const RHOperation& operation) = 0;
	virtual void SetViewport(const RHViewport& vp) = 0;

protected:
	RHFactory* mRHFactory;

	uint32_t mWidth, mHeight;
	bool mIsFullscreen;

	shared_ptr<RHShaderPipeline> mCurrentShaderPipeline;
	shared_ptr<RHFrameBuffer> mCurrentFrameBuffer;
	shared_ptr<RHFrameBuffer> mScreenFrameBuffer;
	shared_ptr<RHBlendState> mCurrentBlendState;
	shared_ptr<RHRasterizerState> mCurrentRasterizerState;
	shared_ptr<RHDepthStencilState> mCurrentDepthStencilState;

	ColorRGBA mCurrentBlendFactor;
	uint32_t mCurrentSampleMask;
	uint16_t mCurrentFrontStencilRef, mCurrentBackStencilRef;	
};

} // Namespace RcEngine

#endif // RHDevice_h__
