#include "Graphics/RenderOperation.h"
#include "Graphics/GraphicBuffer.h"

namespace RcEngine {
	namespace Render {

		RenderOperation::RenderOperation( void )
			: mUseIndex(true), mPrimitiveType(PT_Triangle_List),
			mStartIndexLocation(0), mStartVertexLocation(0), mBaseVertexLocation(0)
		{

		}

		uint32 RenderOperation::GetVertexCount() const
		{
			return 0;
		}

		uint32 RenderOperation::GetStreamCount() const
		{
			return mVertexStreams.size();
		}

		bool RenderOperation::UseIndices() const
		{
			return mUseIndex;
		}

		uint32 RenderOperation::GetIndicesCount() const
		{
			static uint32 sIndexSize[2] = { 2, 4 };
			return mIndexBuffer->GetBufferSize() / sIndexSize[mIndexType];
		}

		RenderOperation::StreamUnit RenderOperation::GetStreamUnit( uint32 index ) const
		{
			return mVertexStreams[index];
		}

		void RenderOperation::SetStreamUnit( uint32 index, RenderOperation::StreamUnit s )
		{
			mVertexStreams[index] = s;
		}

		GraphicsBuffer* RenderOperation::GetIndexStream() const
		{
			return mIndexBuffer;
		}

		uint32 RenderOperation::GetStartVertexLocation() const
		{
			return mStartVertexLocation;
		}

		void RenderOperation::SetStartVertexLocation( uint32 loc )
		{
			mStartVertexLocation = loc;
		}

		uint32 RenderOperation::GetBaseVertexLocation() const
		{
			return mBaseVertexLocation;
		}

		void RenderOperation::SetBaseVertexLocation( uint32 loc )
		{
			mBaseVertexLocation = loc;
		}

		uint32 RenderOperation::GetStartIndexLocation() const
		{
			return mStartIndexLocation;
		}

		void RenderOperation::SetStartIndexLocation( uint32 loc )
		{
			mStartIndexLocation = loc;
		}

		



	} // RenderSystem
} // RcEngine
