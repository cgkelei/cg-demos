#ifndef ShaderProgram_h__
#define ShaderProgram_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class Shader;
class Effect;

class _ApiExport ShaderProgram
{
public:
	ShaderProgram(Effect& effect);
	virtual~ ShaderProgram();

	void AttachShader(ShaderType shaderType, const shared_ptr<Shader>& shader);
	void DetachShader(ShaderType shaderType, const shared_ptr<Shader>& shader);

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	
protected:
	Effect& mEffect;
	shared_ptr<Shader> mShaderStage[ST_Count];
};

}


#endif // ShaderProgram_h__
