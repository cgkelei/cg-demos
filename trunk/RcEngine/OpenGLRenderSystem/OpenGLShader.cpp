#include "OpenGLShader.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

enum GLSLVersion
{
	GLSL400 = 0,
	GLSL410,
	GLSL420,
	GLSL430,
	GLSL440
};

static String GLSLVersion[] = {
	"#version 400\n",
	"#version 410\n",
	"#version 420\n",
	"#version 430\n",
	"#version 440\n"
};

const String& GetSupportedGLSLVersion() 
{
	if (GLEW_VERSION_4_4)
		return GLSLVersion[GLSL440];
	else if (GLEW_VERSION_4_3)
		return GLSLVersion[GLSL430]; 
	else if (GLEW_VERSION_4_2)
		return GLSLVersion[GLSL420]; 
	else if (GLEW_VERSION_4_1)
		return GLSLVersion[GLSL410]; 
	else if (GLEW_VERSION_4_0)
		return GLSLVersion[GLSL400]; 
	else
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Only supported OpenGL 4.0 above hardware!", "GetSupportedGLSLVersion");
}



OpenGLShader::OpenGLShader( ShaderType shaderType )
	: Shader(shaderType),
	  ShaderOGL(0)
{

}

OpenGLShader::~OpenGLShader()
{
	if (ShaderOGL)
	{
		glDeleteShader(ShaderOGL);
		ShaderOGL = 0;
	}
}


bool OpenGLShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{
	ShaderOGL = glCreateShader(OpenGLMapping::Mapping(mShaderType));
		
	String version = GetVersion()























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