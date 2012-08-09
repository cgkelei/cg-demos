#include "Graphics/RenderOperation.h"
#include "Graphics/GraphicBuffer.h"

namespace RcEngine {
	namespace Render {

		RenderOperation::RenderOperation( void )
			: UseIndex(true), PrimitiveType(PT_Triangle_List),
			StartIndexLocation(0), StartVertexLocation(0), BaseVertexLocation(0)
		{

		}


		RenderOperation::~RenderOperation()
		{
		}

		uint32_t RenderOperation::GetVertexCount() const
		{
			return 0;
		}

		uint32_t RenderOperation::GetStreamCount() const
		{
			return VertexStreams.size();
		}

		bool RenderOperation::UseIndices() const
		{
			return UseIndex;
		}

		uint32_t RenderOperation::GetIndicesCount() const
		{
			static uint32_t sIndexSize[2] = { 2, 4 };
			return IndexBuffer->GetBufferSize() / sIndexSize[IndexType];
		}

		uint32_t RenderOperation::GetStartVertexLocation() const
		{
			return StartVertexLocation;
		}

		void RenderOperation::SetStartVertexLocation( uint32_t loc )
		{
			StartVertexLocation = loc;
		}

		uint32_t RenderOperation::GetBaseVertexLocation() const
		{
			return BaseVertexLocation;
		}

		void RenderOperation::SetBaseVertexLocation( uint32_t loc )
		{
			BaseVertexLocation = loc;
		}

		uint32_t RenderOperation::GetStartIndexLocation() const
		{
			return StartIndexLocation;
		}

		void RenderOperation::SetStartIndexLocation( uint32_t loc )
		{
			StartIndexLocation = loc;
		}

		void RenderOperation::BindVertexStream( const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd, StreamType type /*= ST_Geometry*/, uint32_t freq /*= 1*/ )
		{
			if(type == ST_Geometry)
			{
				for(size_t i = 0; i < VertexStreams.size(); i++)
				{
					if( (*VertexStreams[i].VertexDecl) == *vd )
					{
						VertexStreams[i].Stream = buffer;
						VertexStreams[i].Type = type;
						VertexStreams[i].Frequency = freq;
						return;
					}
				}
			}
			StreamUnit su;
			su.Stream = buffer;
			su.VertexDecl = vd;
			su.Frequency = freq;
			su.Type = type;
			VertexStreams.push_back(su);
		}

		void RenderOperation::BindIndexStream( const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type )
		{
			IndexType = type;
			IndexBuffer = buffer;
		}

		const RenderOperation::StreamUnit& RenderOperation::GetStreamUnit( uint32_t index ) const
		{
			return VertexStreams[index];
		}

	}
}
