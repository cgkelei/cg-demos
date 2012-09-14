#ifndef Shader_h__
#define Shader_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class _ApiExport Shader
{
public:
	Shader(ShaderType shaderType);
	virtual ~Shader();
	
	virtual void Release() = 0;

	virtual bool Compile(const String& source, const String& entryPoint = "") = 0; 

	const String& GetCompileInfo() const	{ return mCompileOutput; }
	ShaderType GetShaderType() const  { return mShaderType; }

	bool Validate() const { return mValidate; } 

private:
	Shader( const Shader& );
	Shader& operator= ( const Shader& );

protected:
	ShaderType mShaderType;
	String mShaderSource;
	String mCompileOutput;
	bool mValidate;
};

}


#endif // Shader_h__
