#ifndef OpenGLGraphicCommon_h__
#define OpenGLGraphicCommon_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

enum OpenGLShaderParameterClass
{
	Shader_Param_Uniform = 0,
	Shader_Param_SRV,
	Shader_Param_UAV
};

class _OpenGLExport OpenGLMapping
{
public:
	OpenGLMapping(void);
	~OpenGLMapping(void);

	static GLenum Mapping(FillMode fm);
	static GLenum Mapping(VertexElementFormat format);
	static GLenum Mapping(PrimitiveType type);
	static GLenum Mapping(BlendOperation op);
	static GLenum Mapping(AlphaBlendFactor factor);
	static GLenum Mapping(CompareFunction cf);
	static GLenum Mapping(StencilOperation sop);
	static GLenum Mapping(TextureAddressMode mode);
	static GLenum Mapping(ShaderType type);

	// OpenGL usage
	static GLenum Mapping(uint32_t accessHint);

	static void Mapping(GLenum& min, GLenum& mag, TextureFilter filter);

	// Reference glTexImage* function
	static void Mapping(GLenum& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat);
	static PixelFormat UnMapping( GLenum inInternalformat, GLenum inFormat, GLenum inType);
	static void UnMapping(GLenum glType, EffectParameterType& paramType, OpenGLShaderParameterClass& paramClass);

	/**
	 * Return if the opengl type is interger.
	 */
	static bool IsIntegerType(GLenum type);
};

}



#endif // OpenGLGraphicCommon_h__