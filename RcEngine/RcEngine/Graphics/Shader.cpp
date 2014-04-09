#include <Graphics/Shader.h>

namespace RcEngine {

Shader::Shader( ShaderType shaderType )
	: mShaderType(shaderType)
{

}

Shader::~Shader()
{

}

void Shader::AddInclude( const String& include )
{
	mShaderIncludes += include + "\n";
}

void Shader::AddDefine( const String& name, const String& value )
{
	mDefines.push_back(name);
	mDefineValues.push_back(value);
}

void Shader::DumpSource( const char* filename )
{
	std::ofstream fs(filename);

	mShaderSource.erase(std::remove(mShaderSource.begin(), mShaderSource.end(), '\r'), mShaderSource.end());
	fs << mShaderSource << std::endl;
	fs.close();
}




}