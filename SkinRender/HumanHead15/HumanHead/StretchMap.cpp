#include "StretchMap.h"
#include <nvShaderUtils.h>



GLuint StretchMap::msStretchProgramID;
GLint StretchMap::msScaleParam;
GLint StretchMap::msWorldMatParam;

void StretchMap::Init()
{
	GLuint shaderVS = nv::CompileGLSLShaderFromFile(GL_VERTEX_SHADER, StretchMapVS);
	ASSERT(shaderVS > 0);

	GLuint shaderFS = nv::CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER, StretchMapFS);
	ASSERT(shaderFS > 0);

	msStretchProgramID = nv::LinkGLSLProgram(shaderVS, shaderFS);
	ASSERT(msStretchProgramID > 0);

	msWorldMatParam = glGetUniformLocation(msStretchProgramID, "WorldMat");
	ASSERT(msWorldMatParam >= 0);

	msScaleParam = glGetUniformLocation(msStretchProgramID, "Scale");
	ASSERT(msWorldMatParam >= 0);
}

void StretchMap::Release()
{

}


StretchMap::StretchMap(void)
{
}


StretchMap::~StretchMap(void)
{
}

void StretchMap::Begin()
{

}

void StretchMap::End()
{

}

void StretchMap::SetScale( float scale )
{
	ASSERT(msScaleParam >= 0);
	glUniform1f(msScaleParam, scale);
}

void StretchMap::SetWorldMatrix( const nv::matrix4f& world )
{
	ASSERT(msWorldMatParam >= 0);
	glUniformMatrix4fv(msWorldMatParam, 1, true, world.get_value());
}

