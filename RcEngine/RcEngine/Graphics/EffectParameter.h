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

	uint32_t GetBufferSize() const;
	inline shared_ptr<GraphicsBuffer> GetBuffer() const		{ return mCBuffer; }
	inline const String& GetCBufferName() const				{ return mCBName; }
	inline void MakeDirty()									{ mDirty = true; }
	inline void ClearDirty()								{ mDirty = false; }

	void UpdateBuffer();

public_internal:
	void AddEffectParameter(EffectParameter* parameter, uint32_t offset, uint32_t arraySize, uint32_t arrayStrides, uint32_t matrixStrides); 

protected:
	String mCBName;
	shared_ptr<GraphicsBuffer> mCBuffer;

	struct CBParameter
	{
		EffectParameter* Parameter;
		uint32_t Offset;
		uint32_t ArraySize;
		uint32_t ArrayStrides;
		uint32_t MatrixStrides;
	};
	std::vector<CBParameter> mParameters;

	bool mDirty;
};

class _ApiExport EffectParameter
{
	friend class EffectConstantBuffer;

public:
	EffectParameter(const String& name, EffectParameterType type, EffectConstantBuffer* pCB = nullptr);
	virtual ~EffectParameter(void);

	inline const String& GetName() const					{ return mParameterName; }
	inline EffectParameterType GetParameterType() const		{ return mParameterType; }

	// Clone 
	virtual EffectParameter* Clone() = 0;

	// Set and Get method, call the matched parameter type version, or will cause exception
	virtual void GetValue(bool& value) const;
	virtual void GetValue(vector<bool>& value) const;
	virtual void GetValue(float& value) const;
	virtual void GetValue(vector<float>& value) const;
	virtual void GetValue(int& value) const;
	virtual void GetValue(vector<int32_t>& value) const;
	virtual void GetValue(float4x4& value) const;
	virtual void GetValue(vector<float4x4>& value) const;
	virtual void GetValue(float2& value) const;
	virtual void GetValue(vector<float2>& value) const;
	virtual void GetValue(float3& value) const;
	virtual void GetValue(vector<float3>& value) const;
	virtual void GetValue(float4& value) const;
	virtual void GetValue(vector<float4>& value) const;
	virtual void GetValue(TextureLayer& value) const;
	
	virtual void SetValue(const bool& value);
	virtual void SetValue(const vector<bool>& value);
	virtual void SetValue(const float& value);
	virtual void SetValue(const vector<float>& value);
	virtual void SetValue(const int& value);
	virtual void SetValue(const vector<int32_t>& value);
	virtual void SetValue(const float4x4& value);
	virtual void SetValue(const vector<float4x4>& value);
	virtual void SetValue(const float2& value);
	virtual void SetValue(const vector<float2>& value);
	virtual void SetValue(const float3& value);
	virtual void SetValue(const vector<float3>& value);
	virtual void SetValue(const float4& value);
	virtual void SetValue(const vector<float4>& value);
	virtual void SetValue(const TextureLayer& value);

	// Make constant buffer dirty
	void MakeDirty();

	inline TimeStamp GetTimeStamp() const { return mLastModifiedTime; }

protected:
	String mParameterName;
	EffectParameterType mParameterType;

	bool mDirty;
	TimeStamp mLastModifiedTime;
	EffectConstantBuffer* mConstantBuffer; // Constant buffer this variable belong to
};

template< typename T>
class _ApiExport EffectParameterNumberic : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type) : EffectParameter(name, type) {}
	~EffectParameterNumberic()  { }

	EffectParameter* Clone()
	{
		EffectParameterNumberic<T>* retVal = new EffectParameterNumberic<T>(mParameterName, mParameterType);
		retVal->mValue = mValue;
		return retVal;
	}

	virtual void GetValue(T& value) const { value = mValue; }
	virtual void SetValue(const T& value)
	{
		if (value != mValue)
		{
			mValue = value;
			MakeDirty();
		}
	}

protected:
	T mValue;
};

typedef EffectParameterNumberic<bool>								EffectParameterBool;
typedef EffectParameterNumberic< vector<bool> >						EffectParameterBoolArray;

typedef EffectParameterNumberic<uint32_t>							EffectParameterUInt;
typedef EffectParameterNumberic< vector<uint32_t> >					EffectParameterUIntArray;
typedef EffectParameterNumberic< Vector<uint32_t, 2> >				EffectParameterUInt2;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 2> > >		EffectParameterUInt2Array;
typedef EffectParameterNumberic< Vector<uint32_t, 3> >				EffectParameterUInt3;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 3> > >		EffectParameterUInt3Array;
typedef EffectParameterNumberic< Vector<uint32_t, 4> >				EffectParameterUInt4;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 4> > >		EffectParameterUInt4Array;

typedef EffectParameterNumberic<int32_t>							EffectParameterInt;
typedef EffectParameterNumberic< vector<int32_t> >					EffectParameterIntArray;
typedef EffectParameterNumberic< Vector<int32_t, 2> >				EffectParameterInt2;
typedef EffectParameterNumberic< vector<Vector<int32_t, 2> > >		EffectParameterInt2Array;
typedef EffectParameterNumberic< Vector<int32_t, 3> >				EffectParameterInt3;
typedef EffectParameterNumberic< vector<Vector<int32_t, 3> > >		EffectParameterInt3Array;
typedef EffectParameterNumberic< Vector<int32_t, 4> >				EffectParameterInt4;
typedef EffectParameterNumberic< vector<Vector<int32_t, 4> > >		EffectParameterInt4Array;

typedef EffectParameterNumberic<float> EffectParameterFloat;
typedef EffectParameterNumberic< vector<float> >					EffectParameterFloatArray;
typedef EffectParameterNumberic<float2>								EffectParameterVector2;
typedef EffectParameterNumberic< vector<float2> >					EffectParameterVector2Array;
typedef EffectParameterNumberic<float3>								EffectParameterVector3;
typedef EffectParameterNumberic< vector<float3> >					EffectParameterVector3Array;
typedef EffectParameterNumberic<float4>								EffectParameterVector4;
typedef EffectParameterNumberic< vector<float4> >					EffectParameterVector4Array;

typedef EffectParameterNumberic<float4x4>							EffectParameterMatrix;
typedef EffectParameterNumberic< vector<float4x4> >					EffectParameterMatrixArray;

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

	EffectParameter* Clone()
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
