#include "Graphics/RenderJob.h"
#include "Graphics/VertexDeclaration.h"

namespace RcEngine
{
	namespace Render
	{

		RenderJob::RenderJob()
		{

		}


		void RenderJob::AddVertexStream( const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& decl, uint32_t fre /*=1*/, StreamType type /*= ST_Geometry */ )
		{
			StreamUnit su;
			su.Stream = vb;
			su.VertexDecl = decl;
			su.Frequency = fre;
			su.Type = type;
			su.VertexSize = decl->GetVertexSize();

			VertexStreams.push_back(su);
		}



		RenderJobCollection::RenderJobCollection()
		{

		}

		RenderJobCollection::~RenderJobCollection()
		{

		}
		
}
}
