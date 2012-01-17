// [7/31/2011 hustruan]
#ifndef _Viewport__H
#define _Viewport__H

#include "Core/Prerequisites.h"

namespace RcEngine {
	namespace RenderSystem {

		class  _ApiExport Viewport
		{
		public:
			Viewport();
			Viewport(unsigned int left, unsigned int top, unsigned int width, unsigned int height);
			~Viewport();

			bool operator == (const Viewport& rhs);

		public:

			unsigned int mLeft, mTop, mWidth, mHeight;

		};

	} // RenderSystem
} // RcEngine

#endif // _Viewport__H