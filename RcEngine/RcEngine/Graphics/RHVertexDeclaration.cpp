#include <Graphics/RHVertexDeclaration.h>

namespace RcEngine {

RHVertexElement::RHVertexElement( uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint32_t index /*= 0*/ )
	: Offset(offset),
	  Type(theType),
	  Usage(semantic),
	  UsageIndex(index),
	  InputSlot(0),
	  InstanceStepRate(0)
{

}


RHVertexDeclaration::RHVertexDeclaration( const RHVertexElement* element, uint32_t count )
{
	mVertexElemets.assign(element, element + count);
}

}