#include <Graphics/EffectParameter.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderFactory.h>
#include <Core/Context.h>

namespace RcEngine {

EffectParameter::EffectParameter( const String& name, EffectParameterType type, EffectConstantBuffer* pCB /*= nullptr*/ ) 
	: mParameterType(type), 
	  mParameterName(name), 
	  mConstantBuffer(pCB),
	  mDirty(true)
{

}

EffectParameter::~EffectParameter( void )
{

}

// Overload the specified version in sub-class
void EffectParameter::GetValue(bool& value)	const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<bool>& value) const			 { throw std::exception(); }
void EffectParameter::GetValue(float& value) const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<float>& value) const			 { throw std::exception(); }
void EffectParameter::GetValue(int& value) const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<int32_t>& value) const		 { throw std::exception(); }
void EffectParameter::GetValue(float4x4& value)	const				 { throw std::exception(); }
void EffectParameter::GetValue(vector<float4x4>& value)	const		 { throw std::exception(); }
void EffectParameter::GetValue(float2& value) const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<float2>& value) const			 { throw std::exception(); }
void EffectParameter::GetValue(float3& value) const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<float3>& value) const			 { throw std::exception(); }
void EffectParameter::GetValue(float4& value) const					 { throw std::exception(); }
void EffectParameter::GetValue(vector<float4>& value) const			 { throw std::exception(); }
void EffectParameter::GetValue(TextureLayer& textureLayer) const	 { throw std::exception(); }

void EffectParameter::SetValue(const bool& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<bool>& value)		 { throw std::exception(); }
void EffectParameter::SetValue(const float& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<float>& value)		 { throw std::exception(); }
void EffectParameter::SetValue(const int& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<int32_t>& value)	 { throw std::exception(); }
void EffectParameter::SetValue(const float4x4& value)			 { throw std::exception(); }
void EffectParameter::SetValue(const vector<float4x4>& value)	 { throw std::exception(); }
void EffectParameter::SetValue(const float2& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<float2>& value)		 { throw std::exception(); }
void EffectParameter::SetValue(const float3& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<float3>& value)		 { throw std::exception(); }
void EffectParameter::SetValue(const float4& value)				 { throw std::exception(); }
void EffectParameter::SetValue(const vector<float4>& value)		 { throw std::exception(); }
void EffectParameter::SetValue(const TextureLayer& textureLayer) { throw std::exception(); }

//---------------------------------------------------------------------
void EffectParameter::MakeDirty()
{
	if (mConstantBuffer)
	{
		// CBuffer dirty 
		mConstantBuffer->MakeDirty();
	}

	mDirty = true;
	mLastModifiedTime++;
}

//----------------------------------------------------------------------------------------
EffectConstantBuffer::EffectConstantBuffer( const String& name, uint32_t bufferSize )
	: mCBName(name)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	mCBuffer = factory.CreateConstantBuffer(BU_Dynamic, EAH_CPU_Write | EAH_GPU_Read, NULL);

}

uint32_t EffectConstantBuffer::GetBufferSize() const
{
	return mCBuffer->GetBufferSize();
}

void EffectConstantBuffer::AddEffectParameter( EffectParameter* parameter, uint32_t offset, uint32_t arraySize, uint32_t arrayStrides, uint32_t matrixStrides )
{
	std::vector<CBParameter>::iterator it;
	it = std::find_if(mParameters.begin(), mParameters.end(), [&](const CBParameter& param) {
						return param.Parameter == parameter; });

	if (it != mParameters.end())
	{
		// Check parameter size and offset 
		assert(offset == it->Offset);
		assert(arraySize == it->ArraySize);
		assert(arrayStrides == it->ArrayStrides);
		assert(matrixStrides == it->MatrixStrides);
	}
	else
	{
		CBParameter cbParam;
		cbParam.Parameter = parameter;
		cbParam.Offset = offset;
		cbParam.ArraySize = arraySize;
		cbParam.ArrayStrides = arrayStrides;
		cbParam.MatrixStrides = matrixStrides;
		
		mParameters.push_back(cbParam);
	}	
}

void EffectConstantBuffer::UpdateBuffer()
{
	if (mDirty)
	{
		for (size_t i = 0; i < mParameters.size(); ++i)
		{
		}
	}
}



} // Namespace RcEngine