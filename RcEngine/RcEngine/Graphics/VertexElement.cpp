#include <Graphics/VertexElement.h>
#include <Core/Exception.h>

namespace RcEngine {
namespace Render {

using Core::Exception;

VertexElement::VertexElement(uint32_t offset, VertexElementFormat theType, VertexElementUsage semantic, uint16_t index /*= 0*/ )
	:   Offset(offset),
		Type(theType),
		Usage(semantic),
		UsageIndex(index)
{
	
}

uint32_t VertexElement::GetTypeSize( VertexElementFormat etype )
{
	switch(etype)
	{
	case VEF_Color:
		return sizeof(uint32_t);

	case VEF_Single:
		return sizeof(float);

	case VEF_Vector2:
		return sizeof(float)*2;

	case VEF_Vector3:
		return sizeof(float)*3;

	case VEF_Vector4:
		return sizeof(float)*4;

	case VEF_Short2:
		return sizeof(short)*2;

	case VEF_Short4:
		return sizeof(short)*4;

	case VEF_UByte4:
		return sizeof(unsigned char)*4;

	case VEF_NormalizedShort2:
		return  sizeof(short)*2;

	case VEF_NormalizedShort4:
		return sizeof(short)*4;

	case VEF_HalfVector2:
		return sizeof(float);

	case VEF_HalfVector4:
		return sizeof(float) * 2;
	}
	return 0;
}

uint16_t VertexElement::GetTypeCount( VertexElementFormat etype )
{
	switch(etype)
	{
	case VEF_Color:
		return 1;

	case VEF_Single:
		return 1;

	case VEF_Vector2:
	case VEF_Short2:
	case VEF_NormalizedShort2:
	case VEF_HalfVector2:
		return 2;

	case VEF_Vector3:
		return 3;

	case VEF_Vector4:
	case VEF_Short4:
	case VEF_UByte4:
	case VEF_NormalizedShort4:
		return 4;
	}
	ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid type", 
		"VertexElement::GetTypeCount");
}

bool VertexElement::IsNormalized( VertexElementFormat etype )
{
	switch(etype)
	{
	case VEF_NormalizedShort2:
	case VEF_NormalizedShort4:
		return true;

	default:
		return false;
	}
}


} // Namespace Render
} // Namespace RcEngine