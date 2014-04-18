#pragma once
#ifndef OpenGLRenderDevice_h__
#define OpenGLRenderDevice_h__

#include "OpenGLGraphicCommon.h"
#include <Graphics/RHDevice.h>

namespace RcEngine {

class OpenGLShader;
class OpenGLBuffer;

// Global OpenGL deice
extern OpenGLDevice* gOpenGLDevice;

class _OpenGLExport OpenGLDevice : public RHDevice
{
public:
	OpenGLDevice();
	~OpenGLDevice(void);

	void CreateRenderWindow();

	void GetBlitFBO(GLuint& srcFBO, GLuint& dstFBO);
	
	inline GLuint GetCurrentFBO() const { return mCurrentFBO; }
	void BindFBO(GLuint fbo); 

	// Bind shader to pipeline stage
	void BindVertexShader(GLuint shaderOGL)			{ glUseProgramStages(mProgramPipeline, GL_VERTEX_SHADER_BIT, shaderOGL); }
	void BindTessControlShader(GLuint shaderOGL)    { glUseProgramStages(mProgramPipeline, GL_TESS_CONTROL_SHADER_BIT, shaderOGL); }
	void BindTessEvalShader(GLuint shaderOGL)       { glUseProgramStages(mProgramPipeline, GL_TESS_EVALUATION_SHADER_BIT, shaderOGL); }
	void BindGeometryShader(GLuint shaderOGL)       { glUseProgramStages(mProgramPipeline, GL_GEOMETRY_SHADER_BIT, shaderOGL); }
	void BindPixelShader(GLuint shaderOGL)          { glUseProgramStages(mProgramPipeline, GL_FRAGMENT_SHADER_BIT, shaderOGL); }
	void BindComputeShader(GLuint shaderOGL)        { glUseProgramStages(mProgramPipeline, GL_COMPUTE_SHADER_BIT, shaderOGL); }

	void ToggleFullscreen(bool fs);
	void AdjustProjectionMatrix(float4x4& pOut);

	void SetBlendState(const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);		
	void SetRasterizerState(const shared_ptr<RHRasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state);
	
protected:

	virtual void SetViewport(const RHViewport& vp);
	/*void DoRender( EffectTechnique& tech, RenderOperation& operation);*/

	void DoDraw(const RHOperation& operation);
	
	//void SetupShaderPipeline(const sha)
	void SetupVertexArray(const RHOperation& operation);
	void SetupIndexBuffer(const RHOperation& operation);

private:
	uint32_t mViewportTop, mViewportLeft, mViewportWidth, mViewportHeight;
	shared_ptr<RHSamplerState> mCurrentSamplerStates[MaxSamplerCout];		

	// source and destination blit framebuffer
	GLuint mBlitFBO[2];

	GLuint mCurrentFBO;
	GLuint mProgramPipeline;
};

}


#endif // OpenGLRenderDevice_h__