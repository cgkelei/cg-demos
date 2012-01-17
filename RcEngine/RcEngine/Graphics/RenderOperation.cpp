//#include "RenderOperation.h"
//
//
//RenderOperation::RenderOperation(void)
//{
//}
//
//
//RenderOperation::~RenderOperation(void)
//{
//}
//
//
//void RenderOperation::BindVertexStream( const GraphicBufferPtr& buffer, const VertexDeclaration& vertexDec )
//{
//	for (size_t i = 0; i < mStreams.size(); ++ i)
//	{
//		if (mStreams[i].VertexDecl == vertexDec)
//		{
//			mStreams[i].Source = buffer;
//			mStreams[i].VertexSize = vertexDec.GetVertexSize();
//			return;
//		}
//	}
//
//	StreamSource ss;
//	ss.Source = buffer;
//	ss.VertexDecl = vertexDec;
//	ss.VertexSize = vertexDec.GetVertexSize();
//	mStreams.push_back(ss);
//}
namespace RcEngine {
namespace RenderSystem {

} // RenderSystem
} // RcEngine
