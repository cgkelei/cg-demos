#pragma once
#ifndef RenderOperation_h__
#define RenderOperation_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexDeclaration.h"

namespace RcEngine {
	namespace Render {

		class _ApiExport RenderOperation
		{
		public:
			enum StreamType
			{
				ST_Geometry,
				ST_Instance
			};

			struct StreamUnit
			{
				GraphicsBuffer* Stream;
				VertexDeclaration* VertexDecl;
				uint32 VertexSize;
				uint32 Frequency;
				StreamType Type;

			};

			typedef std::vector<StreamUnit> StreamList;

		public:
			RenderOperation(void);

			PrimitiveType GetPrimitiveType() const							{ return mPrimitiveType; }
			void SetPrimitiveType(PrimitiveType type)						{ mPrimitiveType = type; }

			uint32 GetVertexCount() const;

			uint32 GetStartVertexLocation() const;
			void SetStartVertexLocation(uint32 loc);
			uint32 GetBaseVertexLocation() const;
			void SetBaseVertexLocation(uint32 loc);
			uint32 GetStartIndexLocation() const;
			void SetStartIndexLocation(uint32 loc);

			uint32 GetStreamCount() const;									
			StreamUnit GetStreamUnit(uint32 index) const;					
			void SetStreamUnit(uint32 index, StreamUnit s);

			bool UseIndices() const;
			GraphicsBuffer* GetIndexStream() const;			
			uint32 GetIndicesCount() const;
			IndexBufferType GetIndexType() const							{ return mIndexType; }


		public:

			PrimitiveType mPrimitiveType;

			StreamList mVertexStreams;
			
			bool mUseIndex;
			GraphicsBuffer* mIndexBuffer;
			IndexBufferType mIndexType;

			uint32 mStartVertexLocation;
			uint32 mStartIndexLocation;
			int32  mBaseVertexLocation;
			uint32 mStartInstanceLocation;
		};


	} // RenderSystem
} // RcEngine


#endif // RenderOperation_h__