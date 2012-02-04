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

		void RenderOperation::BindVertexStream( const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd, StreamType type /*= ST_Geometry*/, uint32 freq /*= 1*/ )
		{
			if(type == ST_Geometry)
			{
				for(size_t i = 0; i < mVertexStreams.size(); i++)
				{
					if( (*mVertexStreams[i].VertexDecl) == *vd )
					{
						mVertexStreams[i].Stream = buffer;
						mVertexStreams[i].Type = type;
						mVertexStreams[i].Frequency = freq;
						return;
					}
				}
			}
				StreamUnit su;
				su.Stream = buffer;
				su.VertexDecl = vd;
				su.Frequency = freq;
				su.Type = type;
				mVertexStreams.push_back(su);
		}

		void RenderOperation::BindIndexStream( const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type )
		{
			mIndexType = type;
			mIndexBuffer = buffer;
		}

		const RenderOperation::StreamUnit& RenderOperation::GetStreamUnit( uint32 index ) const
		{
			return mVertexStreams[index];
		}


		//////////////////////////////////////////////////////////////////////////
		/*RenderOperationBuffer::RenderOperationBuffer()
		{

		}

		RenderOperationBuffer::~RenderOperationBuffer()
		{

		}

		void RenderOperation::BindIndexStream( const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type )
		{
			mIndexBuffer = buffer;
			mIndexType = type;
		}
		

		void RenderOperationBuffer::AddRenderOperation( const RenderOperation& rop )
		{
			mRenderOperationList.push_back(rop);
		}

		const RenderOperation& RenderOperationBuffer::GetRenderOperation( uint32 index ) const
		{
			return mRenderOperationList.at(index);
		}

		void RenderOperationBuffer::ClearAll()
		{
			mRenderOperationList.clear();
		}

		RenderOperation& RenderOperationBuffer::operator[]( uint32 index )
		{
			return mRenderOperationList[index];
		}

		const RenderOperation& RenderOperationBuffer::operator[]( uint32 index ) const
		{
			return mRenderOperationList[index];
		}*/

	}
}
