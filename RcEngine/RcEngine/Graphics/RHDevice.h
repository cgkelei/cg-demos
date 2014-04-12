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

class RHShaderPipeline;
class RHRenderFactory;

class _ApiExport RHDevice
{
public:
	RHDevice(RenderDeviceType deviceType);
	virtual ~RHDevice(void);

	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void ToggleFullscreen(bool fs) = 0;

	virtual void CreateRenderWindow(const RenderSettings& settings) = 0;	
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

	virtual void BindShaderPipeline(const shared_ptr<RHShaderPipeline>& pipeline) = 0;

	void Resize(uint32_t width, uint32_t height);

	void BindFrameBuffer(const shared_ptr<FrameBuffer>& fb);

	inline bool Fullscreen() const								  { return mIsFullscreen; }
	inline RenderDeviceType GetRenderDeviceType() const			  { return mDeviceType; }
	inline RHRenderFactory* GetRenderFactory() const			  { return mRenderFactory; }
																  
	inline shared_ptr<FrameBuffer> GetCurrentFrameBuffer() const  { return mCurrentFrameBuffer; }
	inline shared_ptr<FrameBuffer> GetScreenFrameBuffer() const	  { return mScreenFrameBuffer; }

	inline shared_ptr<DepthStencilState> GetCurrentDepthStencilState() const	{ return mCurrentDepthStencilState; }
	inline shared_ptr<BlendState> GetCurrentBlendState() const					{ return mCurrentBlendState; }

	virtual void SetRasterizerState(const shared_ptr<RasterizerState>& state) = 0;
	virtual void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state) = 0;
	virtual void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask) = 0;
	virtual void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0) = 0;

protected:
	virtual void DoBindFrameBuffer(const shared_ptr<FrameBuffer>& fb) = 0;

protected:

	RenderSettings mRenderSettings;
	RenderDeviceType mDeviceType;
	RHRenderFactory* mRenderFactory;

	uint32_t mWidth, mHeight;
	bool mIsFullscreen;

	shared_ptr<FrameBuffer> mCurrentFrameBuffer;
	shared_ptr<FrameBuffer> mScreenFrameBuffer;
	shared_ptr<BlendState> mCurrentBlendState;
	shared_ptr<RasterizerState> mCurrentRasterizerState;
	shared_ptr<DepthStencilState> mCurrentDepthStencilState;

	ColorRGBA mCurrentBlendFactor;
	uint32_t mCurrentSampleMask;
	uint16_t mCurrentFrontStencilRef, mCurrentBackStencilRef;	
	
	shared_ptr<RHShaderPipeline> mCurrentShaderPipeline;
};

} // Namespace RcEngine

#endif // RHDevice_h__
