#pragma once
#include "Common.h"
#include <nvMath.h>

class Utility
{
public:
	typedef std::pair< std::string, std::string> ShaderMacro;

public:
	static void SaveTextureToPfm(const char* file, GLuint tex, int width, int height);
	static void SaveScreenToPfmDepth( const char* file, int width, int height );
	static void SaveTextureToPfmDepth( const char* file, GLuint tex, int width, int height );
	static void SaveTextureToTGA(const char* file, GLuint tex, int width, int height);
	
	static void PrintEffectUniforms(GLuint program);
	static void PrintEffectAttribs(GLuint program);

	static GLuint LoadShaderEffect(const char* vsFile, const char* fsFile, const std::vector<ShaderMacro>* macro = NULL);

	static GLuint LinkShaderProgram( GLuint vertexShader, GLuint fragmentShader);
	static GLuint LoadShader(GLenum type, const char* file, const std::vector<ShaderMacro>* macro = NULL);
		

	static nv::matrix4f Inverse(const nv::matrix4f& mat);

	static void BuildLookAtMatrix(double eyex, double eyey, double eyez,
		double centerx, double centery, double centerz,
		double upx, double upy, double upz,
		float m[16]);
};

