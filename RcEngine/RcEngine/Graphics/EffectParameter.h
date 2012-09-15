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
	uint32_t Unit;
};


class _ApiExport EffectParameter
{
public:
	EffectParameter(const String& name, EffectParameterType type)
		: mDirty(true), mType(type), mName(name) { }

	virtual ~EffectParameter(void) { }

	const String& GetName() const { return mName; }
	EffectParameterType GetParameterType() const  { return mType; }

	virtual void GetValue(bool& value);
	virtual void GetValue(vector<bool>& value);
	virtual void GetValue(float& value);
	virtual void GetValue(vector<float>& value);
	virtual void GetValue(int& value);
	virtual void GetValue(vector<int32_t>& value);
	virtual void GetValue(Matrix4f& value);
	virtual void GetValue(vector<Matrix4f>& value);
	virtual void GetValue(Vector2f& value);
	virtual void GetValue(vector<Vector2f>& value);
	virtual void GetValue(Vector3f& value);
	virtual void GetValue(vector<Vector3f>& value);
	virtual void GetValue(Vector4f& value);
	virtual void GetValue(vector<Vector4f>& value);
	virtual void GetValue(TextureLayer& value);
	
	virtual void SetValue(const bool& value);
	virtual void SetValue(const vector<bool>& value);
	virtual void SetValue(const float& value);
	virtual void SetValue(const vector<float>& value);
	virtual void SetValue(const int& value);
	virtual void SetValue(const vector<int32_t>& value);
	virtual void SetValue(const Matrix4f& value);
	virtual void SetValue(const vector<Matrix4f>& value);
	virtual void SetValue(const Vector2f& value);
	virtual void SetValue(const vector<Vector2f>& value);
	virtual void SetValue(const Vector3f& value);
	virtual void SetValue(const vector<Vector3f>& value);
	virtual void SetValue(const Vector4f& value);
	virtual void SetValue(const vector<Vector4f>& value);
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
typedef EffectParameterNumberic<Vector2f> EffectParameterVector2;
typedef EffectParameterNumberic< vector<Vector2f> > EffectParameterVector2Array;
typedef EffectParameterNumberic<Vector3f> EffectParameterVector3;
typedef EffectParameterNumberic< vector<Vector3f> > EffectParameterVector3Array;
typedef EffectParameterNumberic<Vector4f> EffectParameterVector4;
typedef EffectParameterNumberic< vector<Vector4f> > EffectParameterVector4Array;
typedef EffectParameterNumberic<Matrix4f> EffectParameterMatrix;
typedef EffectParameterNumberic< vector<Matrix4f> > EffectParameterMatrixArray;

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
			mTextureLayer.Unit != value.Unit ||
			mTextureLayer.Sampler != value.Sampler)
		{
			mTextureLayer = value;
			mDirty = true;
		}
	}

private:
	TextureLayer mTextureLayer;
};



} // Namespace RcEngine


#endif // EffectParameter_h__
