#include "OpenGLShader.h"

namespace RcEngine {

static GLenum MapToOpenGL( ShaderType type )
{
	switch (type)
	{
	case ST_Vertex:
		return GL_VERTEX_SHADER;
	case ST_Pixel:
		return GL_FRAGMENT_SHADER;
	case ST_Geomerty:
		return GL_GEOMETRY_SHADER;
	case ST_Compute:
		return GL_COMPUTE_SHADER;
	}
	throw std::exception("Error");
}


OpenGLShader::OpenGLShader( ShaderType shaderType )
	: Shader(shaderType), mOGLShaderObject(0)
{

}

OpenGLShader::~OpenGLShader()
{
	Release();
}
void OpenGLShader::Release()
{
	if (mOGLShaderObject)
	{
		glDeleteShader(mOGLShaderObject);
		mOGLShaderObject = 0;
		mValidate = false;
		mCompileOutput.clear();
	}
}

bool OpenGLShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{
	mShaderSource = source;

	mOGLShaderObject = glCreateShader( MapToOpenGL(mShaderType) );
	if (!mOGLShaderObject)
	{
		mCompileOutput = "Could not create shader object";
		return false;
	}

	String version, extensions, defines;

	// make version and extensions appears before other language
	size_t versionStart = mShaderSource.find("#version");
	if (versionStart != String::npos)
	{
		size_t versionEnd = mShaderSource.find('\n', versionStart);
		if (versionEnd != String::npos)
		{
			version = mShaderSource.substr(versionStart, versionEnd - versionStart+1);
			mShaderSource = mShaderSource.substr(versionEnd);
		}
	}

	size_t extensionStart = mShaderSource.find("#extension");
	if (extensionStart != String::npos)
	{
		size_t extensionEnd = mShaderSource.find('\n', extensionStart);
		if (extensionEnd != String::npos)
		{
			extensions += mShaderSource.substr(extensionStart, extensionEnd - extensionStart+1);
			mShaderSource = mShaderSource.substr(extensionEnd);
			extensionStart = mShaderSource.find("#extension ");
		}
		else 
			extensionStart = String::npos;
	}

	for (unsigned i = 0; i < mDefines.size(); ++i)
		defines += "#define " + mDefines[i] + " " + mDefineValues[i] + "\n";

	mShaderSource = version + extensions + defines + mShaderIncludes + mShaderSource;

	const char* glslSrc = mShaderSource.c_str();
	glShaderSource(mOGLShaderObject, 1, &glslSrc, NULL);
	glCompileShader(mOGLShaderObject);

	int compiled;
	glGetShaderiv(mOGLShaderObject, GL_COMPILE_STATUS, &compiled);
	mValidate = (compiled != 0);

	if (!mValidate)
	{
		int length;
		glGetShaderiv(mOGLShaderObject, GL_INFO_LOG_LENGTH, &length);
		mCompileOutput.resize(length);
		glGetShaderInfoLog(mOGLShaderObject, length, &length, &mCompileOutput[0]);
	}
	else
	{
		mCompileOutput.clear();
	}

	OGL_ERROR_CHECK();
	return mValidate;
}

}