#include "OpenGLShader.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>
#include <Core/Loger.h>
#include <Core/Utility.h>
#include <IO/PathUtil.h>
#include <fstream>
#include <iterator>
#include <set>

namespace RcEngine {

enum GLSLVersion
{
	GLSL420 = 0,
	GLSL430,
	GLSL440
};

static String GLSLVersion[] = {
	"#version 420\n",
	"#version 430\n",
	"#version 440\n",
};

const String& GetSupportedGLSLVersion() 
{
	if (GLEW_VERSION_4_4)
		return GLSLVersion[GLSL440];
	else if (GLEW_VERSION_4_3)
		return GLSLVersion[GLSL430]; 
	else if (GLEW_VERSION_4_2)
		return GLSLVersion[GLSL420]; 
	else
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Only supported OpenGL 4.2 above hardware!", "GetSupportedGLSLVersion");
}

bool HasVersion(const String& source)
{
	return source.find("#version") != std::string::npos;
}

inline bool LoadBinary(const char* filename, GLenum & format, std::vector<uint8_t>& bytecode)
{
	FILE* fp = fopen(filename, "rb");
	if(fp)
	{
		fread(&format, sizeof(GLenum), 1, fp);

		GLint size;
		fread(&size, sizeof(GLint), 1, fp);

		bytecode.resize(size);
		fread(&bytecode[0], size, 1, fp);

		fclose(fp);
		return true;
	}
	return false;
}

class GLSLScriptCompiler
{
public:
	GLSLScriptCompiler(OpenGLShader* shader) : mShader(shader) {}

	bool OpenGLCompile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint)
	{
		std::ifstream fs(filename);
		std::string glslScript( (std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>() );

		size_t shaderSectionBegin, shaderSectionEnd;
		FindShaderSectionRange(glslScript, mShader->mShaderType, entryPoint, shaderSectionBegin, shaderSectionEnd);

		// Parse shader includes
		bool hasInclude = false;

		const char includeToken[] = "#include";
		size_t includeBegin = glslScript.find(includeToken, shaderSectionBegin);
		while (includeBegin != std::string::npos && includeBegin < shaderSectionEnd)
		{
			// Find include file 
			size_t s = glslScript.find('\"', includeBegin);
			size_t e = glslScript.find('\"', s + 1);

			std::string includeName = glslScript.substr(s + 1, e-s-1);
			if (msIncludes.find(includeName) == msIncludes.end())
			{
				std::string includeFile = PathUtil::GetParentPath(filename) + includeName;

				std::ifstream fs(includeFile);
				if (fs.is_open() == false)
				{
					ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, includeFile + " not founded!", "OpenGLCompile");
				}

				std::string includeScript( (std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>() ); 
				glNamedStringARB(GL_SHADER_INCLUDE_ARB, includeName.length(), includeName.c_str(), includeScript.length(), includeScript.c_str());
			
				// Parse sampler state
				ParseSamplerState(includeScript);
			}
			
			includeBegin = glslScript.find(includeToken, includeBegin + sizeof(includeToken)); // Skip current include
			hasInclude = true;
		}

		// Find first real shader code line, exclude #version, #extension, #param
		bool hasVersion = false;
		uint32_t lineNum = 0;

		size_t lineBeign = shaderSectionBegin;
		size_t lineEnd = glslScript.find('\n', shaderSectionBegin);	
		while (lineEnd < shaderSectionEnd && lineEnd != std::string::npos)
		{
			std::string line = glslScript.substr(lineBeign, lineEnd - lineBeign);
			
			if (line.empty())
			{

			}
			else if (line.find("#version") != std::string::npos)
			{
				hasVersion = true;
			}
			else if (line.find("#error") != std::string::npos     || 
					 line.find("#param") != std::string::npos     ||
					 line.find("#extension") != std::string::npos)
			{

			}
			else 
				break;

			++lineNum;

			lineBeign = lineEnd+1;
			lineEnd = glslScript.find('\n', lineBeign);	
		}

		std::string shaderSource;

		// Add version if not exits
		if (!hasVersion)
			shaderSource += GetSupportedGLSLVersion() + "\n";
		
		if (lineNum > 0)
			shaderSource += glslScript.substr(shaderSectionBegin, lineBeign - shaderSectionBegin);

		// Add include extension
		if (hasInclude)
			shaderSource += "#extension GL_ARB_shading_language_include : enable\n";
	
		// Add Macro
		if (macros)
		{
			for (uint32_t i = 0; i < macroCount; ++i)
			{
				shaderSource += "#define " + macros[i].Name + " " + macros[i].Definition + "\n";
			}
		}

		// Add #line
#ifdef _DEBUG
		size_t lineNum0 = std::count(glslScript.begin(), glslScript.begin() + shaderSectionBegin, '\n');
		lineNum += lineNum0 + 1;
		
		static char lineBuffer[100];
		std::sprintf(lineBuffer, "#line %d\n", lineNum);
		shaderSource += lineBuffer;
#endif
		// Add real code
		shaderSource += glslScript.substr(lineBeign, shaderSectionEnd - lineBeign);

		char const* pSource = shaderSource.c_str();
		mShader->mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShader->mShaderType), 1, &pSource);

		int success;
		glGetProgramiv(mShader->mShaderOGL, GL_LINK_STATUS, &success);
		ParseSamplerState(shaderSource);

		if (success != GL_TRUE)
		{
			int length;
			glGetProgramiv(mShader->mShaderOGL, GL_INFO_LOG_LENGTH, &length);

			std::vector<char> compileOutput(length);
			glGetProgramInfoLog(mShader->mShaderOGL, length, &length, &compileOutput[0]);

			fprintf(stderr, "GLSL %s %s compile failed\n\n%s\n\n", filename.c_str(), entryPoint.c_str(), &compileOutput[0]);
			//EngineLogger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);

			std::ofstream ofs("E:/" + entryPoint + ".glsl");
			ofs << shaderSource;
			ofs.close();
		}

		return (success == GL_TRUE);
	} 

	void ParseSamplerState(const std::string& shaderSource)
	{
		// Parse all SamplerState
		const char SamplerToken[] = "#pragma";

		std::string texture, samplerState, line;

		size_t token = shaderSource.find(SamplerToken);
		while (token != std::string::npos)
		{
			token += sizeof(SamplerToken); // skip #pragma

			size_t lineBreak = shaderSource.find('\n', token);
			line = shaderSource.substr(token, lineBreak - token);
			
			size_t colonPos = line.find(':');
			if (colonPos != std::string::npos)
			{
				size_t s = 0, e = colonPos - 1;
				while(!isalpha(line[s])) s++;
				while(!isalpha(line[e])) e--;
				texture = line.substr(s, e - s + 1);	

				s = colonPos+1;
				while(!isalpha(line[s])) s++;
				e = s;
				while(isalpha(line[e])) e++;
				samplerState = line.substr(s, e - s);	

				mShader->mSamplerStates[texture] = samplerState;
			}

			token = shaderSource.find("#pragma", token);
		}
	}

	void FindShaderSectionRange(const String& glslScript, ShaderType shaderStage, const String& entryPoint, size_t& oSectionBegin, size_t& oSectionEnd)
	{
		// find shader section of entry point
		static const char* GLSLShaderToken[] = {
			"[[Vertex=%s]]", "[[TessControl=%s]", "[[TessEval=%s]]", "[[Geometry=%s]]", "[[Fragment=%s]]", "[[Compute=%s]]" 
		};

		// Split into shader section
		char delimiter[255];
		std::sprintf(delimiter, GLSLShaderToken[shaderStage], entryPoint.c_str());	

		size_t tokenBegin = glslScript.find(delimiter);
		if (tokenBegin != std::string::npos)
		{
			// Find the range of this shader section

			size_t tokenEnd = glslScript.find('\n', tokenBegin)+1; 
			size_t nextTokenEnd = glslScript.find("]]", tokenEnd);

			if (nextTokenEnd != std::string::npos)
			{
				// Validate
				size_t nextTokenBegin = glslScript.rfind("[[", nextTokenEnd);

				// Check whether exits = in nextToken
				bool valid = false;
				for (size_t i = nextTokenBegin; i < nextTokenEnd && !valid; ++i)
					valid = (glslScript[i] == '=');

				assert(valid == true);
				oSectionBegin = tokenEnd;
				oSectionEnd = nextTokenBegin;
			}
			else
			{
				oSectionBegin = tokenEnd;
				oSectionEnd = glslScript.length();
			}
		}
		else
		{
			if (glslScript.find("[[") == std::string::npos)
			{
				// No shader stage token, use the whole content as this shader stage
				oSectionBegin = 0;
				oSectionEnd = glslScript.length();
			}
			else
			{
				// Error: exits other shader section, but can't find this shader stage
				std::cerr << "Shader " + entryPoint + " not found!" << std::endl;
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Shader " + entryPoint + " not found!", "OpenGLShader::LoadFromFile");
			}
		}
	}

private:
	OpenGLShader* mShader;
	
	static std::set<std::string> msIncludes;
};

std::set<std::string> GLSLScriptCompiler::msIncludes;

class OpenGLShaderReflection
{
public:
	OpenGLShaderReflection(OpenGLShader* shaderOGL)
		: mShaderOGL(shaderOGL)
	{
		assert(mShaderOGL);
		mShaderProgramID = mShaderOGL->mShaderOGL;
	}

	void ReflectShader()
	{
		if (mShaderOGL->GetShaderType() == ST_Vertex)
			RefectInputParameters();

		ReflectResource();
	}

private:
	void RefectInputParameters()
	{
		enum { MaxNameLen = 256 };
		GLchar name[MaxNameLen];

		GLint size;
		GLsizei actualLen;
		GLenum type;		

		GLint numAttribsInProgram;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_ATTRIBUTES, &numAttribsInProgram);
		for (GLuint i = 0; i < GLuint(numAttribsInProgram); ++i) 
		{
			glGetActiveAttrib(mShaderProgramID, i, MaxNameLen, &actualLen, &size, &type, name);
			// exclude GLSL system attribute
			if ( strstr(name, "gl_") == NULL )
			{
				InputSignature attribute;
#ifdef _DEBUG
				attribute.Name = name;
#endif
				attribute.AttributeSlot = glGetAttribLocation(mShaderProgramID, name);
				OpenGLMapping::UnMapping(type, attribute.Format);
				attribute.ArraySize = size;

				mShaderOGL->mInputSignatures.push_back(attribute);
			}
		}

		std::sort(mShaderOGL->mInputSignatures.begin(), mShaderOGL->mInputSignatures.end(), [&](const InputSignature& lhs, const InputSignature& rhs) {
					return lhs.AttributeSlot < rhs.AttributeSlot;	
				});
	}

	void ReflectResource()
	{
		enum { MaxNameLen = 256 };
		GLchar name[MaxNameLen];
		
		GLint arraySize, arrayStride, matrixStride, unifomOffset, blockSize;;
		GLsizei actualNameLen;
		GLenum type;

		GLint numUniforms, globalBlockIdx = -1;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);
		for (GLuint i = 0; i < GLuint(numUniforms); ++i)
		{
			glGetActiveUniform(mShaderProgramID, i, MaxNameLen, &actualNameLen, &arraySize, &type, name);
			/**
			 * Hack:
			 * OpenGL seems to treat const variable as active uniform with a modified name.
			 * Don't retrieve it.
			 */
			 if (!isalpha(name[0]))
				 continue;

			if (arraySize > 1 && actualNameLen > 3) // Check array type may contain []
			{
				// remove [] if exits
				actualNameLen = std::distance(name, std::find(name, name + actualNameLen, '['));
			}

			// Variable name
			String actualName(&name[0], actualNameLen);

			// Get uniform block for this uniform
			GLint blockIdx;
			glGetActiveUniformsiv(mShaderProgramID, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx);
			if (blockIdx == GL_INVALID_INDEX) 
			{
				// Global uniform, SRV or UAV
				ShaderParameterClass paramClass;
				EffectParameterType paramType;
				OpenGLMapping::UnMapping(type, paramType, paramClass);
					
				if (paramClass == Shader_Param_Uniform)
				{
					UniformParam uniform;

					uniform.Name = actualName;
					uniform.Type = paramType;
					uniform.ArraySize = (arraySize <= 1) ? 0 : arraySize;
					uniform.Location = glGetProgramResourceLocation(mShaderProgramID, GL_UNIFORM, actualName.c_str());

					if (globalBlockIdx == -1)
					{
						// Add global uniform block
						UniformBuffer globalUniformBlock;
						globalBlockIdx = mShaderOGL->mUniformBuffers.size();
						mShaderOGL->mUniformBuffers.resize(globalBlockIdx+1);
						mShaderOGL->mUniformBuffers.back().Location = -1;
					}

					mShaderOGL->mUniformBuffers[globalBlockIdx].BufferVariables.push_back(uniform);
				}
				else 
				{
					// SRV: Texture or TBuffer 
					// UAV: Image or ImageBuffer
					ResouceViewParam viewParam;
					viewParam.Name = actualName;
					viewParam.Type = paramType;
					viewParam.ViewClass = paramClass;
					viewParam.Location = glGetProgramResourceLocation(mShaderProgramID, GL_UNIFORM, actualName.c_str());

					mShaderOGL->mBoundResources.push_back(viewParam);
				}
			}
		}

		// Uniform blocks
		GLint numUniformBlocks;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);
		for (GLuint i = 0; i < GLuint(numUniformBlocks); ++i)
		{
			glGetActiveUniformBlockName(mShaderProgramID, i, 256, NULL, name);
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			
			// binding point
			GLint binding;
			glGetActiveUniformBlockiv(mShaderProgramID, i,  GL_UNIFORM_BLOCK_BINDING, &binding);

			GLint numUniformInBlock;
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniformInBlock);

			std::vector<GLuint> indices(numUniformInBlock);
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)&indices[0]);

			UniformBuffer uniformBufferParam;
			uniformBufferParam.Name = String(name, actualNameLen);
			uniformBufferParam.Location = i;

			for (int k = 0; k < numUniformInBlock; ++k) 
			{
				glGetActiveUniform(mShaderProgramID, i, MaxNameLen, &actualNameLen, &arraySize, &type, name);	
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_OFFSET, &unifomOffset);
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_MATRIX_STRIDE, &matrixStride);

				if (matrixStride > 0)
					assert(matrixStride == sizeof(float) * 4); // Only support float4x4 matrix

				// Check array type may contain []
				if (arraySize > 1 && actualNameLen > 3) 
				{
					// remove [] if exits
					actualNameLen = std::distance(name, std::find(name, name + actualNameLen, '['));
				}

				UniformParam bufferVariable;
				bufferVariable.Name = String(name, actualNameLen);
				bufferVariable.ArraySize = (arraySize <= 1) ? 0 : arraySize;

				ShaderParameterClass paramClass;
				OpenGLMapping::UnMapping(type, bufferVariable.Type, paramClass);
				assert(paramClass == Shader_Param_Uniform);

				uniformBufferParam.BufferVariables.push_back(bufferVariable);
			}

			mShaderOGL->mUniformBuffers.push_back(uniformBufferParam);
		}

		// Shader Storage Blocks
		GLint numSSBO;
		glGetProgramInterfaceiv(mShaderProgramID, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numSSBO);
		for (GLuint i = 0; i < GLuint(numSSBO); ++i)
		{
			glGetProgramResourceName(mShaderProgramID, GL_SHADER_STORAGE_BLOCK, i, MaxNameLen, &actualNameLen, name);
			
			String actualName(name, actualNameLen);
			if (actualName.find("SRV") != String::npos)
			{
				ResouceViewParam srvParam;
				srvParam.Name = actualName.substr(0, actualName.find("SRV"));
				srvParam.Type = EPT_StructureBuffer;
				srvParam.ViewClass = Shader_Param_SRV;
				srvParam.Location = i;

				mShaderOGL->mBoundResources.push_back(srvParam);
			}
			else if (actualName.find("UAV") != String::npos)
			{
				ResouceViewParam uavParam;
				uavParam.Name = actualName.substr(0, actualName.find("UAV"));
				uavParam.Type = EPT_StructureBuffer;
				uavParam.ViewClass = Shader_Param_UAV;
				uavParam.Location = i;

				mShaderOGL->mBoundResources.push_back(uavParam);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "GLSL StructureBuffer must be postfix with SRV or UAV", "ReflectResource");
			}
		}
	}

private:
	OpenGLShader* mShaderOGL;
	GLuint mShaderProgramID;
};

//////////////////////////////////////////////////////////////////////////
OpenGLShader::OpenGLShader( ShaderType shaderType )
	: Shader(shaderType),
	  mShaderOGL(0)
{
}

OpenGLShader::~OpenGLShader()
{
	if (mShaderOGL)
	{
		glDeleteProgram(mShaderOGL);
		mShaderOGL = 0;
	}
}

bool OpenGLShader::LoadFromByteCode( const String& filename )
{
	GLenum format;
	std::vector<uint8_t> byteCode;

	if ( LoadBinary(filename.c_str(), format, byteCode) == false )
	{
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	GLint success;

	mShaderOGL = glCreateProgram();
	glProgramBinary(mShaderOGL, format, &byteCode[0], byteCode.size());
	glProgramParameteri(mShaderOGL, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glGetProgramiv(mShaderOGL, GL_LINK_STATUS, &success);

	if (success != GL_TRUE)
	{
		int length;
		glGetProgramiv(mShaderOGL, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> compileOutput(length);
		glGetProgramInfoLog(mShaderOGL, length, &length, &compileOutput[0]);

		EngineLogger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);
	}

	OGL_ERROR_CHECK();
	return (success == GL_TRUE);
}

bool OpenGLShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint /*= ""*/ )
{
	//std::ifstream fileStream(filename);
	//std::string glslSource( (std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>() );

	//// Split into shader section
	//static const char* GLSLShaderToken[] = {
	//	"[[Vertex=%s]]", "[[TessControl=%s]", "[[TessEval=%s]]", "[[Geometry=%s]]", "[[Fragment=%s]]", "[[Compute=%s]]" 
	//};

	//char delimiter[255];
	//std::sprintf(delimiter, GLSLShaderToken[mShaderType], entryPoint.c_str());	

	//String shaderSource;

	//size_t tokenBegin = glslSource.find(delimiter);
	//if (tokenBegin != std::string::npos)
	//{
	//	size_t tokenEnd = glslSource.find("]]", tokenBegin)+2; // Skip current one
	//	size_t nextTokenEnd = glslSource.find("]]", tokenEnd);

	//	if (nextTokenEnd != std::string::npos)
	//	{
	//		// Validate
	//		size_t nextTokenBegin = glslSource.rfind("[[", nextTokenEnd);

	//		bool valid = false;
	//		for (size_t i = nextTokenBegin; i < nextTokenEnd && !valid; ++i)
	//			valid = (glslSource[i] == '=');

	//		assert(valid == true);
	//		while(isalpha(glslSource[tokenEnd]) == false) tokenEnd++;
	//		shaderSource = glslSource.substr(tokenEnd, nextTokenBegin - tokenEnd);
	//	}
	//	else
	//		shaderSource = glslSource.substr(tokenEnd);
	//}
	//else
	//{
	//	tokenBegin = glslSource.find("[[");
	//	if (tokenBegin == std::string::npos)
	//	{
	//		shaderSource = glslSource;
	//	}
	//	else
	//		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Shader " + entryPoint + " not found!", "OpenGLShader::LoadFromFile");
	//}

	//// Parse all SamplerState
	//{
	//	size_t token = shaderSource.find("#pragma");
	//	while (token != std::string::npos)
	//	{
	//		token += 7; // skip #pragma
	//		
	//		std::string texture, samplerState;
	//		size_t colonPos = shaderSource.find(':', token);
	//		assert(colonPos != std::string::npos);

	//		size_t s = token+1, e= colonPos - 1;
	//		while(!isalpha(shaderSource[s])) s++;
	//		while(!isalpha(shaderSource[e])) e--;
	//		texture = shaderSource.substr(s, e - s + 1);	

	//		s = colonPos+1;
	//		while(!isalpha(shaderSource[s])) s++;
	//		e = s;
	//		while(isalpha(shaderSource[e])) e++;
	//		samplerState = shaderSource.substr(s, e - s);	

	//		mSamplerStates[texture] = samplerState;

	//		token = shaderSource.find("#pragma", e);
	//	}
	//	
	//}

	//if (HasVersion(shaderSource))
	//{
	//	char const* pSource = shaderSource.c_str();
	//	mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 1, &pSource);
	//}
	//else
	//{
	//	const char* pSource[2];

	//	const String& latestVersion = GetSupportedGLSLVersion();
	//	pSource[0] = latestVersion.c_str();
	//	pSource[1] = shaderSource.c_str();

	//	mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 2, pSource);
	//}
	OGL_ERROR_CHECK();
	GLSLScriptCompiler compiler(this);
	if ( compiler.OpenGLCompile(filename, macros, macroCount, entryPoint) )
	{
		OpenGLShaderReflection shaderReflection(this);
		shaderReflection.ReflectShader();
		OGL_ERROR_CHECK();
	}
	
	return true;
}


}