#pragma once
#ifndef RenderOperation_h__
#define RenderOperation_h__

#include "Core/Prerequisites.h"
#include "GraphicsCommon.h"
#include "VertexDeclaration.h"

namespace RcEngine {
	namespace Render {

		class RenderOperation
		{
		public:

		public:
			RenderOperation(void)
				: mUseIndex(true), mPrimitiveType(PT_Triangle_List)
			{

			}

			PrimitiveType mPrimitiveType;
			GraphicBuffer* mVertexBuffer;

			VertexDeclaration mVertexDecl;

			bool mUseIndex;
			GraphicBuffer* mIndexBuffer;
			IndexBufferType mIndexType;

		};


	} // RenderSystem
} // RcEngine


#endif // RenderOperation_h__