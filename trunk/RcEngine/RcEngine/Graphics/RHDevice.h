#ifndef RHDevice_h__
#define RHDevice_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderSettings.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>

#define MaxSamplerCout 16

namespace RcEngine {

enum RenderDeviceType
{
	RD_OpenGL = 0,
	RD_OpenGL_ES,
	RD_Direct3D11,
	RD_Count
};

struct RenderSettings
{
	bool Fullscreen;
	int32_t Left, Top;
	uint32_t Width, Height;
	PixelFormat ColorFormat;
	PixelFormat DepthStencilFormat;
	uint32_t SyncInterval;
	uint32_t SampleCount, SampleQuality;
	RenderDeviceType RHDeviceType;
};

class RHShaderPipeline;
class RHFactory;
class RHFrameBuffer;
class RHOperation;

class _ApiExport RHDevice
{
public:
	RHDevice(const RenderSettings& settings);
	virtual ~RHDevice(void);

	inline bool Fullscreen() const												    { return mIsFullscreen; }
	inline RenderDeviceType GetDeviceType() const							        { return mRenderSettings.RHDeviceType; }
	inline RHFactory* GetFactory() const									        { return mRenderFactory; }
																  
	inline shared_ptr<RHFrameBuffer> GetCurrentFrameBuffer() const					{ return mCurrentFrameBuffer; }
	inline shared_ptr<RHFrameBuffer> GetScreenFrameBuffer() const					{ return mScreenFrameBuffer; }

	virtual void ToggleFullscreen(bool fs) = 0;
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

	virtual void SetRasterizerState(const shared_ptr<RHRasterizerState>& state) = 0;
	virtual void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state) = 0;
	virtual void SetBlendState(const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	virtual void SetDepthStencilState(const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0) = 0;
	virtual void SetViewport()
	void BindFrameBuffer(const shared_ptr<RHFrameBuffer>& fb);
	void BindShaderPipeline(const shared_ptr<RHShaderPipeline>& pipeline);
	void Draw(const RHOperation& operation);

protected:
	virtual void DoBindFrameBuffer(const shared_ptr<RHFrameBuffer>& fb) = 0;
	virtual void DoDraw(const RHOperation& operation) = 0;

protected:

	RenderSettings mRenderSettings;
	RHFactory* mRenderFactory;

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
