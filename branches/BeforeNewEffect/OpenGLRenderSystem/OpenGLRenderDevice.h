#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include "OpenGLGraphicCommon.h"
#include <Graphics/RenderDevice.h>


namespace RcEngine {

class _OpenGLExport OpenGLRenderDevice : public RenderDevice
{
public:
	OpenGLRenderDevice(void);
	~OpenGLRenderDevice(void);

	void CreateRenderWindow(const RenderSettings& settings);

	void Create();
	void Release();

	void ToggleFullscreen(bool fs);
	bool Fullscreen() const;

	void GetBlitFBO(GLuint& srcFBO, GLuint& dstFBO);

	void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);		
	void SetRasterizerState(const shared_ptr<RasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state);

	void AdjustProjectionMatrix(float4x4& pOut);

protected:
	void DoBindFrameBuffer( const shared_ptr<FrameBuffer>& fb );
	void DoRender( EffectTechnique& tech, RenderOperation& operation);

private:
	void BindVertexBufferOGL(const shared_ptr<GraphicsBuffer>& vertexBuffer );
	void BindIndexBufferOGL(const shared_ptr<GraphicsBuffer>& indexBuffer);
			
private:
	uint32_t mViewportTop, mViewportLeft, mViewportWidth, mViewportHeight;
	shared_ptr<SamplerState> mCurrentSamplerStates[MaxSamplerCout];		

	// source and destination blit framebuffer
	GLuint mBlitFBO[2];
};

}


#endif // OpenGLRenderDevice_h__