#ifndef OpenGLGraphicCommon_h__
#define OpenGLGraphicCommon_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLMapping
		{
		public:
			OpenGLMapping(void);
			~OpenGLMapping(void);

			static GLenum Mapping(VertexElementFormat format);
			static GLenum Mapping(PrimitiveType type);
			

			// Reference glTexImage* function
			static void Mapping(GLint& outInternalformat, GLenum& outFormat, GLenum& outType, PixelFormat inPixelFormat);
			static PixelFormat UnMapping( GLint inInternalformat, GLenum inFormat, GLenum inType);

		};
	}
}



#endif // OpenGLGraphicCommon_h__