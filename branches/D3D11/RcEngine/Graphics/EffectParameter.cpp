#include <Graphics/EffectParameter.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderFactory.h>
#include <Core/Context.h>
#include <Core/Exception.h>

namespace RcEngine {

#if defined(_DEBUG) && !defined(_M_X64)

// This variable indicates how many ticks to go before rolling over
// all of the timer variables in the FX system.
// It is read from the system registry in debug builds; in retail the high bit is simply tested.

_declspec(selectany) unsigned int g_TimerRolloverCount = 0x80000000;

#endif // _DEBUG && !_M_X64

EffectParameter::EffectParameter( const String& name, EffectParameterType type, EffectConstantBuffer* pCB)
	: mParameterType(type), 
	  mParameterName(name), 
	  mConstantBuffer(pCB),
	  mElementSize(1),
	  mCBOffset(0),
	  mLastModifiedTime(0)
{

}

EffectParameter::~EffectParameter( void )
{

}

// Overload the specified version in sub-class
void EffectParameter::GetValue( bool& value ) const 
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( bool*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( int& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( int32_t*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float4x4& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float4x4*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float2& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float2*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float3& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float3*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float4& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( float4*& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::GetValue( TextureLayer& value ) const
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::GetValue"); }

void EffectParameter::SetValue( const bool& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const bool* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const int& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const int32_t* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float4x4& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float4x4* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float2& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float2* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float3& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float3* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float4& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const float4* value, uint32_t count )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetValue( const TextureLayer& value )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetValue"); }

void EffectParameter::SetArrayStride( uint32_t stride )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetArrayStride"); }

void EffectParameter::SetMatrixStride( uint32_t matStride )
{ ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't call this£¡", "EffectParameter::SetMatrixStride"); }

void EffectParameter::IncrementTimeStamp()
{
	mLastModifiedTime++;

#ifndef _M_X64
#if _DEBUG
	if (mLastModifiedTime > g_TimerRolloverCount)
	{
		printf("Rolling over timer (current time: %d, rollover cap: %d).", mLastModifiedTime, g_TimerRolloverCount);
#else
	if (mLastModifiedTime >= 0x80000000) // check to see if we've exceeded ~2 billion
	{
#endif
		// Rollover
		mLastModifiedTime = 1;
	}
#endif // _M_X64
}

void EffectParameter::SetConstantBuffer( EffectConstantBuffer* cbuffer, uint32_t offset )
{
	assert(mConstantBuffer == nullptr);
	mConstantBuffer = cbuffer;
	mCBOffset = offset;
}

void EffectParameter::MakeDirty()
{
	IncrementTimeStamp();
	if (mConstantBuffer)
		mConstantBuffer->MakeDirty();
}

//----------------------------------------------------------------------------------------
EffectConstantBuffer::EffectConstantBuffer( const String& name, uint32_t bufferSize )
	: mCBName(name), mBufferSize(bufferSize)
{
	mBackingStore = new uint8_t[bufferSize];

	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ElementInitData initData;
	initData.pData = NULL;
	initData.rowPitch = bufferSize;
	mCBuffer = factory.CreateConstantBuffer(BU_Dynamic, EAH_CPU_Write | EAH_GPU_Read, &initData);
}

EffectConstantBuffer::~EffectConstantBuffer()
{
	delete[] mBackingStore;
}

void EffectConstantBuffer::AddEffectParameter( EffectParameter* parameter, uint32_t offset )
{
	std::vector<EffectParameter*>::iterator it;
	it = std::find_if(mParameters.begin(), mParameters.end(), [&](const EffectParameter* param) {
						return param == parameter; });
	
	if (it == mParameters.end())
	{
		parameter->SetConstantBuffer(this, offset);
		mParameters.push_back(parameter);
	}
}

void EffectConstantBuffer::UpdateBuffer()
{
	if (mDirty)
	{
		void* pBuffer = mCBuffer->Map(0, MAP_ALL_BUFFER, BA_Write_Only);
		memcpy(pBuffer, mBackingStore, mBufferSize);
		mCBuffer->UnMap();
		mDirty = false;
	}
}

void EffectConstantBuffer::SetRawValue( const void* pData, uint32_t offset, uint32_t count )
{
#ifdef _DEBUG
	if ((offset + count < offset) || (offset + (uint8_t*)pData < (uint8_t*)pData) ||
		((offset + count) > mBufferSize))
	{
		// overflow of some kind
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid range specified", "EffectConstantBuffer::SetRawValue");
	}
#endif

	memcpy(mBackingStore + offset, pData, count);
	mDirty = true;
}

void EffectConstantBuffer::GetRawValue( void *pData, uint32_t offset, uint32_t count )
{
#ifdef _DEBUG
	if ((offset + count < offset) ||
		(count + (uint8_t*)pData < (uint8_t*)pData) ||
		((offset + count) > mBufferSize))
	{
		// overflow of some kind
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid range specified", "EffectConstantBuffer::GetRawValue");
	}
#endif

	memcpy(pData, mBackingStore + offset, count);
}



} // Namespace RcEngine