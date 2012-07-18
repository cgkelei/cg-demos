#include "VertexDeclaration.h"

namespace RcEngine {
namespace Render {

VertexDeclaration::VertexDeclaration()
{

}

VertexDeclaration::~VertexDeclaration()
{

}

const VertexElement* VertexDeclaration::FindElementBySemantic( VertexElementUsage sem, uint16_t index /*= 0*/ )
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

uint32_t VertexDeclaration::GetVertexSize() const
{
	uint32_t size = 0;
	VertexElementList::const_iterator ei, eiend;
	eiend = mElementList.end();
	for (ei = mElementList.begin(); ei != eiend; ++ei)
	{
		size += ei->GetSize();
	}

	return size;
}

const VertexElement& VertexDeclaration::AddElement(uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint16_t index /*= 0*/ )
{
	mElementList.push_back(
		VertexElement(offset, theType, semantic, index)
		);
	return mElementList.back();
}

void VertexDeclaration::AddElement( const VertexElement& ve )
{
	mElementList.push_back(ve);
}

const VertexElement& VertexDeclaration::InsertElement( uint32_t atPosition, uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint16_t index /*= 0*/ )
{
	if ( atPosition >= static_cast<uint32_t>(mElementList.size()) )
	{
		return AddElement(offset, theType, semantic, index);
	}

	VertexElementList::iterator i = mElementList.begin();
	for (uint32_t n = 0; n < atPosition; ++n)
		++i;

	i = mElementList.insert(i,  VertexElement( offset, theType, semantic, index));
	return *i;
}

void VertexDeclaration::RemoveElement( uint32_t elemIndex )
{
	assert(elemIndex < static_cast<uint32_t>(mElementList.size()) && "Index out of bounds");
	VertexElementList::iterator i = mElementList.begin();
	for (uint32_t n = 0; n < elemIndex; ++n)
		++i;
	mElementList.erase(i);
}

void VertexDeclaration::RemoveElement( VertexElementUsage semantic, uint16_t index /*= 0*/ )
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


shared_ptr<VertexDeclaration> CreateVertexDeclaration( VertexDeclarationDesc* inputElementDescs, uint32_t numElements )
{
	shared_ptr<VertexDeclaration> vd = std::make_shared<VertexDeclaration>();
	for(uint32_t i = 0; i < numElements; i++)
	{
		vd->AddElement(inputElementDescs[i].Offset, inputElementDescs[i].Format, inputElementDescs[i].Usage, inputElementDescs[i].UsageIndex);
	}
	return vd;
}

shared_ptr<VertexDeclaration> CreateVertexDeclaration( std::vector<VertexElement> elems )
{
	shared_ptr<VertexDeclaration> vd = std::make_shared<VertexDeclaration>();
	for (size_t i = 0; i < elems.size(); ++i)
	{
		vd->AddElement(elems[i]);
	}
	return vd;
}

} // RenderSystem
} // RcEngine
