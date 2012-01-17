#include "RenderPatameter.h"
#include "Core/Exception.h"

namespace RcEngine {
namespace RenderSystem {

using Core::Exception;



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
void RenderParameter::SetValue( const int& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int, 2>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int, 3>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<int, 4>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const unsigned int& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<unsigned int, 2>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<unsigned int, 3>& value )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}

void RenderParameter::SetValue( const Vector<unsigned int, 4>& value )
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

void RenderParameter::SetValue( const Texture* tex )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Error", "RenderParameter::SetValue");
}


} // RenderSystem
} // RcEngine
