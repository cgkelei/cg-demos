#include "RenderFactory.h"

namespace RcEngine {
namespace Render {

RenderFactory::RenderFactory(void)
{
}


RenderFactory::~RenderFactory(void)
{
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexDeclarationDesc* inputElementDescs, unsigned int numElements )
{
	shared_ptr<VertexDeclaration> vd = std::make_shared<VertexDeclaration>();
	for(unsigned int i = 0; i < numElements; i++)
	{
		vd->AddElement(inputElementDescs[i].Offset, inputElementDescs[i].Format, inputElementDescs[i].Usage, inputElementDescs[i].UsageIndex);
	}
	return vd;
}


} // RenderSystem
} // RcEngine
