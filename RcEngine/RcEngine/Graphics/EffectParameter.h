#ifndef EffectParameter_h__
#define EffectParameter_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

typedef size_t TimeStamp;

struct TextureLayer
{
	shared_ptr<Texture> Texture;
	shared_ptr<SamplerState> Sampler;
	ShaderType Stage;
	uint32_t TexUnit;
};

class _ApiExport EffectConstantBuffer
{
public:
	EffectConstantBuffer(const String& name, uint32_t bufferSize);
	~EffectConstantBuffer();

	uint32_t GetBufferSize() const							{ return mBufferSize; }
	inline shared_ptr<GraphicsBuffer> GetBuffer() const		{ return mCBuffer; }
	inline const String& GetCBufferName() const				{ return mCBName; }
	inline void MakeDirty()									{ mDirty = true; }
	inline void ClearDirty()								{ mDirty = false; }
	inline void* GetRawData(uint32_t offset) const			{ return mBackingStore + offset; }

	void UpdateBuffer();
	void SetRawValue(const void* pData, uint32_t offset, uint32_t count);
	void GetRawValue(void *pData, uint32_t offset, uint32_t count);

public_internal:
	void AddEffectParameter(EffectParameter* parameter); 

protected:
	String mCBName;
	uint8_t* mBackingStore;
	uint32_t mBufferSize;
	shared_ptr<GraphicsBuffer> mCBuffer;
	std::vector<EffectParameter*> mParameters;

	bool mDirty;
};

class _ApiExport EffectParameter
{
	friend class EffectConstantBuffer;

public:
	EffectParameter(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr);
	virtual ~EffectParameter(void);

	inline uint32_t GetElementSize() const					{ return mElementSize; }
	inline const String& GetName() const					{ return mParameterName; }
	inline EffectParameterType GetParameterType() const		{ return mParameterType; }

	// Clone 
	virtual EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr) = 0;

	// Set and Get method, call the matched parameter type version, or will cause exception
	virtual void GetValue(bool& value) const;
	virtual void GetValue(bool*& value) const;
	virtual void GetValue(float& value) const;
	virtual void GetValue(float*& value) const;
	virtual void GetValue(int& value) const;
	virtual void GetValue(int32_t*& value) const;
	virtual void GetValue(float4x4& value) const;
	virtual void GetValue(float4x4*& value) const;
	virtual void GetValue(float2& value) const;
	virtual void GetValue(float2*& value) const;
	virtual void GetValue(float3& value) const;
	virtual void GetValue(float3*& value) const;
	virtual void GetValue(float4& value) const;
	virtual void GetValue(float4*& value) const;
	virtual void GetValue(TextureLayer& value) const;
	
	virtual void SetValue(const bool& value);
	virtual void SetValue(const bool* value, uint32_t count);
	virtual void SetValue(const float& value);
	virtual void SetValue(const float* value, uint32_t count);
	virtual void SetValue(const int& value);
	virtual void SetValue(const int32_t* value, uint32_t count);
	virtual void SetValue(const float4x4& value);
	virtual void SetValue(const float4x4* value, uint32_t count);
	virtual void SetValue(const float2& value);
	virtual void SetValue(const float2* value, uint32_t count);
	virtual void SetValue(const float3& value);
	virtual void SetValue(const float3* value, uint32_t count);
	virtual void SetValue(const float4& value);
	virtual void SetValue(const float4* value, uint32_t count);
	virtual void SetValue(const TextureLayer& value);

public_internal:
	// Make constant buffer dirty
	inline TimeStamp GetTimeStamp() const					{ return mLastModifiedTime; }
	inline uint32_t GetCBOffset() const						{ return mCBOffset; }
	
	void SetCBOffset(uint32_t offset);
	void SetConstantBuffer(EffectConstantBuffer* cbuffer);
	
	void MakeDirty();

	virtual void SetArrayStride(uint32_t stride);
	virtual void SetMatrixStride(uint32_t matStride);

protected:
	void IncrementTimeStamp();

protected:
	String mParameterName;
	EffectParameterType mParameterType;
	
	TimeStamp mLastModifiedTime;

	// For non-array type, always 1.
	uint32_t mElementSize;
	
	// Constant buffer this variable belong to
	EffectConstantBuffer* mConstantBuffer;

	// Offset in parent constant buffer
	uint32_t mCBOffset;
};

template< typename T>
class _ApiExport EffectParameterNumberic : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) {}
	~EffectParameterNumberic()  { }

	EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr)
	{
		EffectParameterNumberic<T>* retVal = new EffectParameterNumberic<T>(mParameterName, mParameterType, pCB);
		retVal->mValue = mValue;
		return retVal;
	}

	virtual void GetValue(T& value) const { value = mValue; }
	virtual void SetValue(const T& value)
	{
		if (value != mValue)
		{
			mValue = value;
			
			// Update data in constant buffer
			if (mConstantBuffer)
			{
				*(reinterpret_cast<T*>(mConstantBuffer->GetRawData(mCBOffset))) = mValue;
			}

			MakeDirty();
		}
	}

protected:
	T mValue;
};

template< typename T >
class _ApiExport EffectParameterNumbericArray : public EffectParameter
{
public:
	EffectParameterNumbericArray(const String& name, EffectParameterType type, uint32_t arrSize, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB)  {  mElementSize = arrSize; mValue = new T[arrSize]; }
	~EffectParameterNumbericArray()  { delete[] mValue; }

	EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr)
	{
		EffectParameterNumbericArray<T>* retVal = new EffectParameterNumbericArray<T>(mParameterName, mParameterType, mElementSize, pCB);
		memcpy(retVal->mValue, mValue, sizeof(T) * mElementSize);
		return retVal;
	}

	virtual void SetArrayStride(uint32_t stride) { mArrayStrides = stride; }
	virtual void GetValue(T*& value) const { value = mValue; }
	virtual void SetValue(const T* value, uint32_t count)
	{
		assert(count <= mElementSize);
		if (memcmp(value, mValue, sizeof(T) * count) != 0)
		{
			memcpy(mValue, value, sizeof(T) * count);
			
			// Update data in constant buffer
			if (mConstantBuffer)
			{
				uint8_t* pData = static_cast<uint8_t*>( mConstantBuffer->GetRawData(mCBOffset) );

				if (mArrayStrides == sizeof(T))
					memcpy(pData, mValue, sizeof(T) * count);
				else 
				{
					for (uint32_t i = 0; i < count; ++i)
						memcpy(pData + i * mArrayStrides, mValue + i, sizeof(T));
				}
			}

			MakeDirty();
		}
	}

protected:
	T* mValue;

	// Array strides in parent constant buffer
	uint32_t mArrayStrides;
};

// Matrix type need more attention
template<>
class _ApiExport EffectParameterNumberic<float4x4> : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) {}
	~EffectParameterNumberic()  { }

	EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr)
	{
		EffectParameterNumberic<float4x4>* retVal = new EffectParameterNumberic<float4x4>(mParameterName, mParameterType, pCB);
		retVal->mValue = mValue;
		return retVal;
	}

	virtual void SetMatrixStride(uint32_t stride) { mMatrixStride = stride; }
	virtual void GetValue(float4x4& value) const { value = mValue; }
	virtual void SetValue(const float4x4& value)
	{
		if (value != mValue)
		{
			mValue = value;

			// Update data in constant buffer
			if (mConstantBuffer)
			{
				// Column-major matrix in shader 
				*(reinterpret_cast<float4x4*>(mConstantBuffer->GetRawData(mCBOffset))) = mValue.Transpose();		
			}

			MakeDirty();
		}
	}

protected:
	float4x4 mValue;
	uint32_t mMatrixStride;
};

template<>
class _ApiExport EffectParameterNumbericArray<float4x4> : public EffectParameter
{
public:
	EffectParameterNumbericArray(const String& name, EffectParameterType type, uint32_t arrSize, EffectConstantBuffer* pCB = nullptr) 
		: EffectParameter(name, type, pCB) { mElementSize = arrSize; mValue = new float4x4[arrSize]; }
	~EffectParameterNumbericArray()  { delete[] mValue; }

	EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr)
	{
		EffectParameterNumbericArray<float4x4>* retVal = new EffectParameterNumbericArray<float4x4>(mParameterName, mParameterType, mElementSize, pCB);
		memcpy(retVal->mValue, mValue, sizeof(float4x4) * mElementSize);
		return retVal;
	}

	virtual void SetMatrixStride(uint32_t stride) { mMatrixStride = stride; }
	virtual void SetArrayStride(uint32_t stride) { mArrayStrides = stride; }
	virtual void GetValue(float4x4*& value) const { value = mValue; }
	virtual void SetValue(const float4x4* value, uint32_t count)
	{
		assert(count <= mElementSize);
		if (memcmp(value, mValue, sizeof(float4x4) * count) != 0)
		{
			// OpenGL global uniform, OpenGLAPI will handle it
			memcpy(mValue, value, sizeof(float4x4) * count);

			// Update data in constant buffer
			if (mConstantBuffer)
			{
				assert(mMatrixStride == sizeof(float4));
				assert(count <= mElementSize);
				/**
				 * Both GLSL and HLSL matrix use column-major as default pack ordering. So there has
				 * two choice. Transpose every matrix before sending to ConstantBuffer, or use row_major 
				 * qualifier in shader language.
				 */
				float4x4* pMatrix = reinterpret_cast<float4x4*>( mConstantBuffer->GetRawData(mCBOffset) );
				for (uint32_t i = 0; i < count; ++i)
					pMatrix[i] = mValue[i].Transpose();		
			}

			MakeDirty();
		}
	}

protected:
	float4x4* mValue;

	// Array strides in parent constant buffer
	uint32_t mArrayStrides;

	// Matrix row or column stride in constant buffer
	uint32_t mMatrixStride;
};


typedef EffectParameterNumberic< bool >								EffectParameterBool;
typedef EffectParameterNumberic< uint32_t >							EffectParameterUInt;
typedef EffectParameterNumberic< uint2 >							EffectParameterUInt2;
typedef EffectParameterNumberic< uint3 >							EffectParameterUInt3;
typedef EffectParameterNumberic< uint4 >							EffectParameterUInt4;
typedef EffectParameterNumberic< int32_t >							EffectParameterInt;
typedef EffectParameterNumberic< int2 >								EffectParameterInt2;
typedef EffectParameterNumberic< int3 >								EffectParameterInt3;
typedef EffectParameterNumberic< int4 >								EffectParameterInt4;
typedef EffectParameterNumberic< float >							EffectParameterFloat;
typedef EffectParameterNumberic< float2 >							EffectParameterVector2;
typedef EffectParameterNumberic< float3 >							EffectParameterVector3;
typedef EffectParameterNumberic< float4 >							EffectParameterVector4;
typedef EffectParameterNumberic< float4x4 >							EffectParameterMatrix;

typedef EffectParameterNumbericArray< bool >						EffectParameterBoolArray;
typedef EffectParameterNumbericArray< uint32_t >					EffectParameterUIntArray;
typedef EffectParameterNumbericArray< int32_t>						EffectParameterIntArray;
typedef EffectParameterNumbericArray< uint2 >						EffectParameterUInt2Array;
typedef EffectParameterNumbericArray< uint3 >						EffectParameterUInt3Array;
typedef EffectParameterNumbericArray< uint4 >						EffectParameterUInt4Array;
typedef EffectParameterNumbericArray< int2 >						EffectParameterInt2Array;
typedef EffectParameterNumbericArray< int3 >						EffectParameterInt3Array;
typedef EffectParameterNumbericArray< int4 >						EffectParameterInt4Array;
typedef EffectParameterNumbericArray< float >						EffectParameterFloatArray;
typedef EffectParameterNumbericArray< float2 >						EffectParameterVector2Array;
typedef EffectParameterNumbericArray< float3 >						EffectParameterVector3Array;
typedef EffectParameterNumbericArray< float4 >						EffectParameterVector4Array;
typedef EffectParameterNumbericArray< float4x4 >					EffectParameterMatrixArray;

// Texture parameter 
class _ApiExport EffectParameterTexture : public EffectParameter
{
public:
	EffectParameterTexture(const String& name, EffectParameterType type)
		: EffectParameter(name, type) { }
	~EffectParameterTexture() { }

	void GetValue(TextureLayer& value)
	{
		value = mTextureLayer;
	}

	void SetValue(const TextureLayer& value)
	{
		if (mTextureLayer.Texture != value.Texture ||
			mTextureLayer.Stage != value.Stage ||
			mTextureLayer.TexUnit != value.TexUnit ||
			mTextureLayer.Sampler != value.Sampler)
		{
			mTextureLayer = value;
			MakeDirty();
		}
	}

	EffectParameter* Clone(EffectConstantBuffer* pCB = nullptr)
	{
		EffectParameterTexture* retVal = new EffectParameterTexture(mParameterName, mParameterType);

		retVal->mTextureLayer.TexUnit = mTextureLayer.TexUnit;
		retVal->mTextureLayer.Stage = mTextureLayer.Stage;
		retVal->mTextureLayer.Texture = mTextureLayer.Texture;
		retVal->mTextureLayer.Sampler = mTextureLayer.Sampler;

		return retVal;
	}

private:
	TextureLayer mTextureLayer;
};



} // Namespace RcEngine


#endif // EffectParameter_h__
