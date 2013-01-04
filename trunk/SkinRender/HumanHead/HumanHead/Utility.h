#pragma once
#include "Common.h"
#include <nvMath.h>

class Utility
{
public:
	typedef std::pair< std::string, std::string> ShaderMacro;

public:
	static void SaveTextureToPfm(const char* file, GLuint tex, int width, int height);
	static void SaveScreenToPfm( const char* file, int width, int height );
	static void SaveTextureToPfmDepth( const char* file, GLuint tex, int width, int height );
	static void SaveTextureToTGA(const char* file, GLuint tex, int width, int height);

	static void WriteTGARGBA(const char* file, int width, int height, float* src);
	
	static void PrintEffectUniforms(GLuint program);
	static void PrintEffectAttribs(GLuint program);

	static GLuint LoadShaderEffect(const char* vsFile, const char* fsFile, const std::vector<ShaderMacro>* macro = NULL);

	static GLuint LinkShaderProgram( GLuint vertexShader, GLuint fragmentShader);
	static GLuint LoadShader(GLenum type, const char* file, const std::vector<ShaderMacro>* macro = NULL);

	
		
//	 std: tring formatString(const char *fmt, ...) {
//	char tmp[512];
//	va_list iterator;
//
//#if defined(WIN32)
//	va_start(iterator, fmt);
//	size_t size = _vscprintf(fmt, iterator) + 1;
//
//	if (size >= sizeof(tmp)) {
//		char *dest = new char[size];
//		vsnprintf_s(dest, size, size-1, fmt, iterator);
//		va_end(iterator);
//		std: tring result(dest);
//		delete[] dest;
//		return result;
//	}
//
//	vsnprintf_s(tmp, size, size-1, fmt, iterator);
//	va_end(iterator);
//#else
//	va_start(iterator, fmt);
//	size_t size = vsnprintf(tmp, sizeof(tmp), fmt, iterator);
//	va_end(iterator);
//
//	if (size >= sizeof(tmp)) {
//		/* Overflow! -- dyna
// mically allocate memory */
//		char *dest = new char[size+1];
//		va_start(iterator, fmt);
//		vsnprintf(dest, size+1, fmt, iterator);
//		va_end(iterator);
//
//		std: tring result(dest);
//		delete[] dest;
//		return result;
//	}
//#endif
//
//	return std: tring(tmp);
//}

};

