#include <Graphics/ShaderProgram.h>
#include <Graphics/Shader.h>
#include <Graphics/Effect.h>

namespace RcEngine {

ShaderProgram::ShaderProgram(Effect& effect)
	: mEffect(effect)
{

}

ShaderProgram::~ ShaderProgram()
{
	
}

void ShaderProgram::AttachShader( ShaderType shaderType, const shared_ptr<Shader>& shader )
{
	mShaderStage[shaderType] = shader;
}

void ShaderProgram::DetachShader( ShaderType shaderType, const shared_ptr<Shader>& shader )
{
	mShaderStage[shaderType] = nullptr;
}

}