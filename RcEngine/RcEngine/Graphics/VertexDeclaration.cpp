#include "VertexDeclaration.h"

namespace RcEngine {
namespace Render {

VertexDeclaration::VertexDeclaration()
{

}

VertexDeclaration::~VertexDeclaration()
{

}

const VertexElement* VertexDeclaration::FindElementBySemantic( VertexElementUsage sem, uint16 index /*= 0*/ )
{
	VertexElementList::const_iterator ei, eiend;
	eiend = mElementList.end();
	for (ei = mElementList.begin(); ei != eiend; ++ei)
	{
		if (ei->GetUsage() == sem && ei->GetUsageIndex() == index)
		{
			return &(*ei);
		}
	}
	return NULL;
}

uint32 VertexDeclaration::GetVertexSize() const
{
	uint32 size = 0;
	VertexElementList::const_iterator ei, eiend;
	eiend = mElementList.end();
	for (ei = mElementList.begin(); ei != eiend; ++ei)
	{
		size += ei->GetSize();
	}

	return size;
}

const VertexElement& VertexDeclaration::AddElement(uint32 offset, VertexElementFormat theType, VertexElementUsage semantic, uint16 index /*= 0*/ )
{
	mElementList.push_back(
		VertexElement(offset, theType, semantic, index)
		);
	return mElementList.back();
}

const VertexElement& VertexDeclaration::InsertElement( uint32 atPosition, uint32 offset, VertexElementFormat theType, VertexElementUsage semantic, uint16 index /*= 0*/ )
{
	if ( atPosition >= static_cast<uint32>(mElementList.size()) )
	{
		return AddElement(offset, theType, semantic, index);
	}

	VertexElementList::iterator i = mElementList.begin();
	for (uint32 n = 0; n < atPosition; ++n)
		++i;

	i = mElementList.insert(i,  VertexElement( offset, theType, semantic, index));
	return *i;
}

void VertexDeclaration::RemoveElement( uint32 elemIndex )
{
	assert(elemIndex < static_cast<uint32>(mElementList.size()) && "Index out of bounds");
	VertexElementList::iterator i = mElementList.begin();
	for (uint32 n = 0; n < elemIndex; ++n)
		++i;
	mElementList.erase(i);
}

void VertexDeclaration::RemoveElement( VertexElementUsage semantic, uint16 index /*= 0*/ )
{
	VertexElementList::iterator i = mElementList.begin();
	for( ; i!= mElementList.end(); ++i)
	{
		if(i->GetUsage() == semantic && i->GetUsageIndex() == index)
		{
			mElementList.erase(i);
			return;
		}
	}
}

void VertexDeclaration::RemoveAllElements( void )
{
	mElementList.clear();
}

bool VertexDeclaration::VertexElementLess( const VertexElement& e1, const VertexElement& e2 )
{
	
	// Use ordering of semantics to sort
	if (e1.GetUsage() < e2.GetUsage())
	{
		return true;
	}
	else if (e1.GetUsage() == e2.GetUsage())
	{
		// Use index to sort
		if (e1.GetUsageIndex() < e2.GetUsageIndex())
		{
			return true;
		}
	}
	
	return false;
}

const VertexElementList& VertexDeclaration::GetElements( void ) const
{
	return mElementList;
}

void VertexDeclaration::Sort( void )
{
	std::sort(mElementList.begin(), mElementList.end(), VertexDeclaration::VertexElementLess);
}


} // RenderSystem
} // RcEngine
