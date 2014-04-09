#ifndef Shader_h__
#define Shader_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

typedef std::pair<String, String> ShaderMacro;

class _ApiExport Shader
{
public:
	Shader(ShaderType shaderType);
	virtual ~Shader();
	
	inline ShaderType GetShaderType() const		{ return mShaderType; }

	virtual bool Compile(const std::vector<uint8_t>& bytecode) = 0;

	virtual bool Complie(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "") = 0;

private:
	Shader( const Shader& );
	Shader& operator= ( const Shader& );

protected:
	ShaderType mShaderType;
};

}


#endif // Shader_h__
