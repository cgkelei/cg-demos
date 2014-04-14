#include "OpenGLShader.h"
#include "OpenGLGraphicCommon.h"
//#include "OpenGLDevice.h"
#include <Core/Exception.h>
#include <Core/Loger.h>

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

//////////////////////////////////////////////////////////////////////////
OpenGLShader::OpenGLShader( ShaderType shaderType )
	: RHShader(shaderType),
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

		EngineLoger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);
	}

	OGL_ERROR_CHECK();
	return (success == GL_TRUE);
}

bool OpenGLShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	assert(GLEW_ARB_separate_shader_objects);

	String shaderSource;

	for (const ShaderMacro& macro : macros)
	{
		shaderSource += "#define " + macro.Name + " " + macro.Definition + "\r\n";
	}

	// Add #line 1
	//shaderSource += "#line 1";
	
	// Load source code into string
	std::ifstream fileStream(filename);
	shaderSource.append((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	if (HasVersion(shaderSource))
	{
		char const* pSource = shaderSource.c_str();

		mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 1, &pSource);
	}
	else
	{
		const char* pSource[2];

		const String& latestVersion = GetSupportedGLSLVersion();
		pSource[0] = latestVersion.c_str();
		pSource[1] = shaderSource.c_str();

		mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 2, pSource);
	}
	
	int success;
	glGetProgramiv(mShaderOGL, GL_LINK_STATUS, &success);
	
	if (success != GL_TRUE)
	{
		int length;
		glGetProgramiv(mShaderOGL, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> compileOutput(length);
		glGetProgramInfoLog(mShaderOGL, length, &length, &compileOutput[0]);

		EngineLoger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);
	}

	OGL_ERROR_CHECK();
	return (success == GL_TRUE);
}

void OpenGLShader::ShaderReflect()
{
	GLint size, location, maxNameLen;
	GLsizei nameLen;
	GLenum type;
	
	// Get max character length for active uniform maxNameLen
	glGetProgramiv(mShaderOGL, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);
	std::vector<GLchar> name(maxNameLen);
	
	// Active uniforms in each block
	std::vector<std::vector<String>> blockVariableNames;

	GLint numUniformsInProgram;
	glGetProgramiv(mShaderOGL, GL_ACTIVE_UNIFORMS, &numUniformsInProgram);
	for (GLuint i = 0; i < GLuint(numUniformsInProgram); ++i)
	{
		glGetActiveUniform(mShaderOGL, i, maxNameLen, &nameLen, &size, &type, &name[0]);
		
		/**
		 * Hack:
		 * OpenGL seems to treat const variable as active uniform with a modified name.
		 * Don't retrieve it.
		 */
		 if (!isalpha(name[0]))
			 continue;

		// Check array type
		if (size > 1 && nameLen > 3) // must contain []
		{
			// Get variable name without []
			nameLen = std::distance(name.begin(), std::find(name.begin(), name.begin() + nameLen, '['));
		}

		// Variable name
		std::string actualName(&name[0], nameLen);

		// Get uniform block for this uniform
		GLint blockIdx;
		glGetActiveUniformsiv(mShaderOGL, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx);
		if (blockIdx == GL_INVALID_INDEX) // Global uniform parameter
		{
			UniformParameter uniformParameter;

			uniformParameter.Name = actualName;
			uniformParameter.Type = OpenGLMapping::UnMapping(type);
			uniformParameter.ArraySize = size;
			uniformParameter.Location = glGetProgramResourceLocation(mShaderOGL, GL_UNIFORM, &name[0]);
			//uniformParameter.Location = glGetUniformLocation(ShaderOGL, &name[0]);
			uniformParameter.Offset = -1;

			
		}
		else
		{
			// Uniform block
			if (blockVariableNames.size() < size_t(blockIdx + 1))
			{
				blockVariableNames.resize(blockIdx + 1);
			}

			blockVariableNames[blockIdx].push_back(actualName);
		}
	}

	GLint numBlocks;
	glGetProgramiv(mShaderOGL, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	assert(blockVariableNames.size() == numBlocks);

	if (numBlocks > 0)
	{
		GLint blockSize, numUniformInBlock;
		std::vector<GLuint> indices;
		std::vector<GLint> offset;
		std::vector<GLint> types;
		std::vector<GLint> arraySize;
		std::vector<GLint> arrayStrides;
		std::vector<GLint> matrixStrides;

		for (GLuint i = 0; i < GLuint(blockVariableNames.size()); ++i)
		{
			glGetActiveUniformBlockiv(mShaderOGL, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &maxNameLen); 
			name.resize(maxNameLen);

			// Get uniform block name
			glGetActiveUniformBlockName(mShaderOGL, i, maxNameLen, &nameLen, &name[0]);

			glGetActiveUniformBlockiv(mShaderOGL, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniformInBlock); 
			assert(blockVariableNames[i].size() == numUniformInBlock);

			glGetActiveUniformBlockiv(mShaderOGL, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

			//glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
			//glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformIndices);

			indices.resize(numUniformInBlock);
			offset.resize(numUniformInBlock);
			types.resize(numUniformInBlock);
			arraySize.resize(numUniformInBlock);
			arrayStrides.resize(numUniformInBlock);
			matrixStrides.resize(numUniformInBlock);

			std::vector<const GLchar*> pBlockVariableNames(blockVariableNames[i].size());
			for (size_t j = 0; j < pBlockVariableNames.size(); ++j)
				pBlockVariableNames[j] = blockVariableNames[i][j].c_str();

			glGetUniformIndices(mShaderOGL, numUniformInBlock, &pBlockVariableNames[0], &indices[0]);
			glGetActiveUniformsiv(mShaderOGL, numUniformInBlock, &indices[0], GL_UNIFORM_OFFSET, &offset[0]);
			glGetActiveUniformsiv(mShaderOGL, numUniformInBlock, &indices[0], GL_UNIFORM_TYPE, &types[0]);
			glGetActiveUniformsiv(mShaderOGL, numUniformInBlock, &indices[0], GL_UNIFORM_SIZE, &arraySize[0]);
			glGetActiveUniformsiv(mShaderOGL, numUniformInBlock, &indices[0], GL_UNIFORM_ARRAY_STRIDE, &arrayStrides[0]);
			glGetActiveUniformsiv(mShaderOGL, numUniformInBlock, &indices[0], GL_UNIFORM_MATRIX_STRIDE, &matrixStrides[0]);

			GLuint blockIdx = glGetUniformBlockIndex(mShaderOGL, &name[0]);

			// Can specify binding slot in GLSL ?
			glUniformBlockBinding(mShaderOGL, blockIdx, i);

			for (size_t j = 0; j < blockVariableNames[i].size(); ++j)
			{
				UniformParameter uniformBlockParameter;
				
				uniformBlockParameter.Name = blockVariableNames[i][j];
				uniformBlockParameter.Location = -1;

				uniformBlockParameter.Offset = offset[j];
				uniformBlockParameter.ArraySize = arraySize[j];
				uniformBlockParameter.Type = OpenGLMapping::UnMapping(types[j]);
				uniformBlockParameter.MatrixStride = matrixStrides[j];
				uniformBlockParameter.ArrayStride = arrayStrides[j];
			}
		}
	}
	
	OGL_ERROR_CHECK();
}

//////////////////////////////////////////////////////////////////////////
//OpenGLShaderPipeline::OpenGLShaderPipeline( Effect& effect )
//	: ShaderProgram(effect)
//{
//
//}
//
//OpenGLShaderPipeline::~OpenGLShaderPipeline()
//{
//
//}
//
//void OpenGLShaderPipeline::Bind()
//{
//	OpenGLDevice& deviceOGL = *(static_cast<OpenGLDevice*>(Context::GetSingleton().GetRenderDevicePtr()));
//
//	if (mShaderStage[ST_Vertex])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Vertex].get()))->ShaderOGL;
//		deviceOGL.BindVertexShader(shaderOGL);
//		// input layout
//	}
//
//	if (mShaderStage[ST_Hull])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Hull].get()))->ShaderOGL;
//		deviceOGL.BindTessControlShader(shaderOGL);
//	}
//
//	if (mShaderStage[ST_Domain])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Domain].get()))->ShaderOGL;
//		deviceOGL.BindTessEvalShader(shaderOGL);
//	}
//
//	if (mShaderStage[ST_Geomerty])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Geomerty].get()))->ShaderOGL;
//		deviceOGL.BindGeometryShader(shaderOGL);
//	}
//
//	if (mShaderStage[ST_Pixel])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Pixel].get()))->ShaderOGL;
//		deviceOGL.BindPixelShader(shaderOGL);
//	}
//
//	if (mShaderStage[ST_Compute])
//	{
//		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStage[ST_Compute].get()))->ShaderOGL;
//		deviceOGL.BindComputeShader(shaderOGL);
//	}
//}
//
//void OpenGLShaderPipeline::Unbind()
//{
//	OpenGLDevice& deviceOGL = *(static_cast<OpenGLDevice*>(Context::GetSingleton().GetRenderDevicePtr()));
//
//	if (mShaderStage[ST_Vertex])	deviceOGL.BindVertexShader(0);
//	if (mShaderStage[ST_Hull])		deviceOGL.BindTessControlShader(0);
//	if (mShaderStage[ST_Domain])	deviceOGL.BindTessEvalShader(0);
//	if (mShaderStage[ST_Geomerty])	deviceOGL.BindGeometryShader(0);
//	if (mShaderStage[ST_Pixel])		deviceOGL.BindPixelShader(0);
//	if (mShaderStage[ST_Compute])	deviceOGL.BindComputeShader(0);
//}


}