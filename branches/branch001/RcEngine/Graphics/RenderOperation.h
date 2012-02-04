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
				shared_ptr<GraphicsBuffer> Stream;
				shared_ptr<VertexDeclaration> VertexDecl;
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
			const StreamUnit& GetStreamUnit(uint32 index) const;
			
			void BindVertexStream(const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd,
				StreamType type = ST_Geometry, uint32 freq = 1);
			void BindIndexStream(const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type);

			bool UseIndices() const;
			uint32 GetIndicesCount() const;

			IndexBufferType GetIndexType() const									{ return mIndexType; }
			const shared_ptr<GraphicsBuffer>& GetIndexStream() const				{ return mIndexBuffer; }			
			


		public:

			PrimitiveType mPrimitiveType;

			StreamList mVertexStreams;
			
			bool mUseIndex;
			shared_ptr<GraphicsBuffer> mIndexBuffer;
			IndexBufferType mIndexType;

			uint32 mStartVertexLocation;
			uint32 mStartIndexLocation;
			int32  mBaseVertexLocation;
			uint32 mStartInstanceLocation;
		};


		/*class _ApiExport RenderOperationBuffer 
		{
		public:
			RenderOperationBuffer();
			~RenderOperationBuffer();

			void AddRenderOperation(const RenderOperation& rop);
			template <typename ForwardIterator>
			void AddRenderOperation(ForwardIterator first, ForwardIterator last)
			{
				for (ForwardIterator iter = first; iter != last; ++iter)
				{
					mRenderOperationList.push_back(*iter);
				}
			}

			const RenderOperation& GetRenderOperation(uint32 index) const;
			RenderOperation& operator[] (uint32 index);
			const RenderOperation& operator[] (uint32 index) const ;

			uint32 GetRenderOperationsCount() const;
			void ClearAll();



		private:
			typedef std::vector<RenderOperation> RenderOperationList;
			RenderOperationList mRenderOperationList;
		};*/


	} // RenderSystem
} // RcEngine


#endif // RenderOperation_h__