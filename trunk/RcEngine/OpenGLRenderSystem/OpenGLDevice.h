#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include <Graphics/RenderDevice.h>
#include "OpenGLGraphicCommon.h"
#include "OpenGLFrameBuffer.h"

namespace RcEngine {

class OpenGLShader;
class OpenGLBuffer;

// Global OpenGL deice
extern OpenGLDevice* gOpenGLDevice;

class _OpenGLExport OpenGLDevice : public RenderDevice
{
public:
	OpenGLDevice();
	~OpenGLDevice(void);

	void CreateRenderWindow();

	void GetBlitFBO(GLuint& srcFBO, GLuint& dstFBO);
	
	inline GLuint GetCurrentFBO() const { return mCurrentFBO; }
	void BindFBO(GLuint fbo); 

	// Bind shader to pipeline stage
	//void BindVertexShader(GLuint shaderOGL)			{ glUseProgramStages(mProgramPipeline, GL_VERTEX_SHADER_BIT, shaderOGL); }
	//void BindTessControlShader(GLuint shaderOGL)    { glUseProgramStages(mProgramPipeline, GL_TESS_CONTROL_SHADER_BIT, shaderOGL); }
	//void BindTessEvalShader(GLuint shaderOGL)       { glUseProgramStages(mProgramPipeline, GL_TESS_EVALUATION_SHADER_BIT, shaderOGL); }
	//void BindGeometryShader(GLuint shaderOGL)       { glUseProgramStages(mProgramPipeline, GL_GEOMETRY_SHADER_BIT, shaderOGL); }
	//void BindPixelShader(GLuint shaderOGL)          { glUseProgramStages(mProgramPipeline, GL_FRAGMENT_SHADER_BIT, shaderOGL); }
	//void BindComputeShader(GLuint shaderOGL)        { glUseProgramStages(mProgramPipeline, GL_COMPUTE_SHADER_BIT, shaderOGL); }

	void ToggleFullscreen(bool fs);
	void AdjustProjectionMatrix(float4x4& pOut);

	void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);		
	void SetRasterizerState(const shared_ptr<RasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state);
	void SetViewports(const std::vector<Viewport>& viewports);

protected:

	void DoDraw(const EffectTechnique* technique, const RenderOperation& operation);
	
private:
	
	// Only track the first view port
	Viewport mCurrentViewport;

	shared_ptr<SamplerState> mCurrentSamplerStates[MaxSamplerCout];		

	// source and destination blit framebuffer
	GLuint mBlitFBO[2];

	GLuint mCurrentFBO;
};

}


#endif // OpenGLRenderDevice_h__