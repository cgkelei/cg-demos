#ifndef Shader_h__
#define Shader_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class Effect;

class _ApiExport Shader
{
public:
	typedef std::pair<String, String> Macro;

public:
	Shader(ShaderType shaderType);
	virtual ~Shader();
	
	inline ShaderType GetShaderType() const		{ return mShaderType; }

	virtual bool LoadFromByteCode(const String& filename) = 0;
	virtual bool LoadFromFile(const String& filename, const std::vector<Macro>& macros, const String& entryPoint = "") = 0;

private:
	Shader( const Shader& );
	Shader& operator= ( const Shader& );

protected:
	ShaderType mShaderType;
};

/**
 * Combination of shader stages.
 */
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


#endif // Shader_h__
