#ifndef EffectParameter_h__
#define EffectParameter_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class EffectParameter;

struct TextureLayer
{
	shared_ptr<Texture> Texture;
	shared_ptr<SamplerState> Sampler;
	ShaderType Stage;
	uint32_t TexUnit;
};


class _ApiExport EffectParameter
{
public:
	EffectParameter(const String& name, EffectParameterType type)
		: mDirty(true), mType(type), mName(name) { }

	virtual ~EffectParameter(void) { }

	virtual EffectParameter* Clone() = 0;

	const String& GetName() const { return mName; }
	EffectParameterType GetParameterType() const  { return mType; }

	virtual void GetValue(bool& value);
	virtual void GetValue(vector<bool>& value);
	virtual void GetValue(float& value);
	virtual void GetValue(vector<float>& value);
	virtual void GetValue(int& value);
	virtual void GetValue(vector<int32_t>& value);
	virtual void GetValue(float4x4& value);
	virtual void GetValue(vector<float4x4>& value);
	virtual void GetValue(float2& value);
	virtual void GetValue(vector<float2>& value);
	virtual void GetValue(float3& value);
	virtual void GetValue(vector<float3>& value);
	virtual void GetValue(float4& value);
	virtual void GetValue(vector<float4>& value);
	virtual void GetValue(TextureLayer& value);
	
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

	void ClearDirty() { mDirty = false; }
	bool Dirty() const { return mDirty; }

protected:
	bool mDirty;
	String mName;
	EffectParameterType mType;
	bool mArray;
};

template< typename T>
class _ApiExport EffectParameterNumberic : public EffectParameter
{
public:
	EffectParameterNumberic(const String& name, EffectParameterType type) : EffectParameter(name, type) {}
	~EffectParameterNumberic()  { }

	EffectParameter* Clone()
	{
		EffectParameterNumberic<T>* retVal = new EffectParameterNumberic<T>(mName, mType);
		
		retVal->mArray = mArray;
		retVal->mValue = mValue;

		return retVal;
	}


	virtual void SetValue(const T& value)
	{
		if (value != mValue)
		{
			mValue = value;
			mDirty = true;
		}
	}

	virtual void GetValue(T& value)
	{
		value = mValue;
	}

protected:
	T mValue;
};

typedef EffectParameterNumberic<bool> EffectParameterBool;
typedef EffectParameterNumberic< vector<bool> > EffectParameterBoolArray;

typedef EffectParameterNumberic<uint32_t> EffectParameterUInt;
typedef EffectParameterNumberic< vector<uint32_t> > EffectParameterUIntArray;
typedef EffectParameterNumberic< Vector<uint32_t, 2> > EffectParameterUInt2;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 2> > > EffectParameterUInt2Array;
typedef EffectParameterNumberic< Vector<uint32_t, 3> > EffectParameterUInt3;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 3> > > EffectParameterUInt3Array;
typedef EffectParameterNumberic< Vector<uint32_t, 4> > EffectParameterUInt4;
typedef EffectParameterNumberic< vector<Vector<uint32_t, 4> > > EffectParameterUInt4Array;

typedef EffectParameterNumberic<int32_t> EffectParameterInt;
typedef EffectParameterNumberic< vector<int32_t> > EffectParameterIntArray;
typedef EffectParameterNumberic< Vector<int32_t, 2> > EffectParameterInt2;
typedef EffectParameterNumberic< vector<Vector<int32_t, 2> > > EffectParameterInt2Array;
typedef EffectParameterNumberic< Vector<int32_t, 3> > EffectParameterInt3;
typedef EffectParameterNumberic< vector<Vector<int32_t, 3> > > EffectParameterInt3Array;
typedef EffectParameterNumberic< Vector<int32_t, 4> > EffectParameterInt4;
typedef EffectParameterNumberic< vector<Vector<int32_t, 4> > > EffectParameterInt4Array;

typedef EffectParameterNumberic<float> EffectParameterFloat;
typedef EffectParameterNumberic< vector<float> > EffectParameterFloatArray;
typedef EffectParameterNumberic<float2> EffectParameterVector2;
typedef EffectParameterNumberic< vector<float2> > EffectParameterVector2Array;
typedef EffectParameterNumberic<float3> EffectParameterVector3;
typedef EffectParameterNumberic< vector<float3> > EffectParameterVector3Array;
typedef EffectParameterNumberic<float4> EffectParameterVector4;
typedef EffectParameterNumberic< vector<float4> > EffectParameterVector4Array;

typedef EffectParameterNumberic<float4x4> EffectParameterMatrix;
typedef EffectParameterNumberic< vector<float4x4> > EffectParameterMatrixArray;

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
			mDirty = true;
		}
	}

	EffectParameter* Clone()
	{
		EffectParameterTexture* retVal = new EffectParameterTexture(mName, mType);

		retVal->mDirty = mDirty;
		retVal->mArray = mArray;

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
