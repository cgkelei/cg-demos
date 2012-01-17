#include "Graphics/Viewport.h"

namespace RcEngine {
	namespace RenderSystem {

		Viewport::Viewport( )	
		{

		}

		Viewport::Viewport( unsigned int left, unsigned int top, unsigned int width, unsigned int height )
		{

		}


		Viewport::~Viewport()
		{

		}

		bool Viewport::operator==( const Viewport& rhs )
		{
			if ((mLeft != rhs.mLeft) || (mTop != rhs.mTop) || 
				(mWidth != rhs.mWidth) || (mHeight != rhs.mHeight))
				return false;
			return true;
		}

	} // RenderSystem
} // RcEngine