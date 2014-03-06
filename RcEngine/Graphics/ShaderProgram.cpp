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
	mAttachedShaders.clear();
}

void ShaderProgram::AttachShader( const shared_ptr<Shader>& shader )
{
	mAttachedShaders.push_back(shader);
}

void Release()
{

}

}