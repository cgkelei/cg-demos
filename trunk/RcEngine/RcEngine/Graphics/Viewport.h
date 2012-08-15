// [7/31/2011 hustruan]
#ifndef _Viewport__H
#define _Viewport__H

#include <Core/Prerequisites.h>

namespace RcEngine {
namespace Render {

class  _ApiExport Viewport
{
public:
	Viewport();
	Viewport(uint32_t left, uint32_t top, uint32_t width, uint32_t height);
	~Viewport();

	bool operator == (const Viewport& rhs);

public:

	uint32_t Left, Top, Width, Height;
	Camera* AttachCamera;

};

} // Namespace Render
} // Namespace RcEngine

#endif // _Viewport__H