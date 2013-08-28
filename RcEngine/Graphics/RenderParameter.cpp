#include "RenderPatameter.h"
#include "Core/Exception.h"

namespace RcEngine {
namespace Render {

using Core::Exception;


String RenderParameter::TypeNames[RenderParameter::PT_Count] = {
	"bool",
	"float",
	"float2",
	"float3",
	"float4",
	"int32",
	"int2",
	"int3",
	"int4",
	"uint",
	"uint2",
	"uint3",
	"uint4",
	"float2x2",
	"float3x3",
	"float4x4"
	"sampler"
};

RenderParameter::RenderParameter()
	: mIsValid(false)
{

}

RenderParameter::~RenderParameter()
{

}

RenderParameter::ParamType RenderParameter::GetParameterType() const
{
	return mParameterType;
}

bool RenderParameter::HasSemantic() const
{
	return !mParameterSemantic.empty();
}

const std::string& RenderParameter::GetParameterName() const
{
	return mParameterName;
}

const std::string& RenderParameter::GetParameterSemantic() const
{
	return mParameterSemantic;
}


void RenderParameter::SetValue( const bool& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}
void RenderParameter::SetValue( const int32_t& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int32_t, 2>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int32_t, 3>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int32_t, 4>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const uint32_t& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<uint32_t, 2>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<uint32_t, 3>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<uint32_t, 4>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const float& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<float, 2>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<float, 3>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<float, 4>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Matrix4<float>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue(const ColorRGBA& value)
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue(const shared_ptr<Texture>& tex )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}


} // RenderSystem
} // RcEngine
