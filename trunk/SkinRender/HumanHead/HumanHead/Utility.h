#pragma once
#include "Common.h"
#include <nvMath.h>

class Utility
{
public:
	static void SaveTextureToPfm(const char* file, GLuint tex, int width, int height);
	static void SaveTextureToPfmDepth( const char* file, GLuint tex, int width, int height );
	static void SaveTextureToTGA(const char* file, GLuint tex, int width, int height);
	
	static void PrintEffectUniforms(GLuint program);
	static void PrintEffectAttribs(GLuint program);

	static GLuint LoadShaderEffect(const char* vsFile, const char* fsFile);

	static nv::matrix4f Inverse(const nv::matrix4f& mat);

	static void BuildLookAtMatrix(double eyex, double eyey, double eyez,
		double centerx, double centery, double centerz,
		double upx, double upy, double upz,
		float m[16]);
};

