#ifndef OpenGLGraphicCommon_h__
#define OpenGLGraphicCommon_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

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

	static void Mapping(GLenum& min, GLenum& mag, TextureFilter filter);

	// Reference glTexImage* function
	static void Mapping(GLint& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat);
	static PixelFormat UnMapping( GLint inInternalformat, GLenum inFormat, GLenum inType);
	static void UnMapping(EffectParameterType& outType, GLenum glType);
};

}



#endif // OpenGLGraphicCommon_h__