#ifndef EffectParameter_h__
#define EffectParameter_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {
namespace Render {
		
using Math::Matrix4f;
using Math::Vector2f;
using Math::Vector3f;
using Math::Vector4f;
using Math::ColorRGBA;

class EffectParameter;

class _ApiExport EffectParameterCollection
{
public:
	EffectParameterCollection();
	~EffectParameterCollection();
		
	void Add(EffectParameter* param);

	EffectParameter* GetParameterByIndex ( uint32_t index) const; 
	EffectParameter* GetParameterByName (const String& name) const;
	EffectParameter* GetParameterBySemantic (const String& semantic) const;

private:
	vector< EffectParameter* > mParams;
};

	
class _ApiExport EffectParameter
{
public:
	EffectParameter();
	virtual ~EffectParameter();

	EffectParameterType GetParameterType() const 			{ return mParameterType; }
	bool HasSemantic() const								{ return !mSemantic.empty(); }
	const String& GetSemantic() const						{ return mSemantic; }
	const String& GetName() const						    { return mName; }
	EffectParameterClass GetParameterClass() const			{ return mParameterClass; }

	/// <summary>
	/// Gets the array elements of the parameter, if it is an array.
	/// </summary>
	virtual EffectParameterCollection* GetElements() const = 0;

	/// <summary>
	/// Gets the structure members of the parameter, if it is a struct.
	/// </summary>
	virtual EffectParameterCollection* StructureMembers() const = 0; 

	/// <summary>
	/// Gets the value of the parameter as a bool.
	/// </summary>
	/// <returns>Bool value</returns>
	virtual bool GetValueBoolean() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of bools.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The bool array</returns>
	virtual vector<bool> GetValueBooleanArray(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a single.
	/// </summary>
	/// <returns>The single value</returns>
	virtual float GetValueFloat() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of singles.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The single array</returns>
	virtual vector<float> GetValueFloatArray(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as an int.
	/// </summary>
	/// <returns>The int value</returns>
	virtual int32_t GetValueInt() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of ints.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The int array</returns>
	virtual vector<int32_t> GetValueIntArray(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a matrix.
	/// </summary>
	/// <returns>The matrix value</returns>
	virtual Matrix4f GetValueMatrix() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of matrices.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The matrix array</returns>
	virtual vector<Matrix4f> GetValueMatrixArray(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a matrix transpose.
	/// </summary>
	/// <returns>The matrix transpose value</returns>
	virtual Matrix4f GetValueMatrixTranspose() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of matrix transposes.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The matrix transpose array</returns>
	virtual vector<Matrix4f> GetValueMatrixTransposeArray(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a RGBA color.
	/// </summary>
	/// <returns>The RGBA color value</returns>
	virtual ColorRGBA GetValueColor() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of RGBA colors.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The quaternion array</returns>
	virtual vector<ColorRGBA> GetValueColorArray(int32_t count) const = 0;


	/// <summary>
	/// Gets the value of the parameter as a vector2.
	/// </summary>
	/// <returns>The vector2 value</returns>
	virtual Vector2f GetValueVector2() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of vector2's.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The vector2 array</returns>
	virtual vector<Vector2f> GetValueVector2Array(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a vector3.
	/// </summary>
	/// <returns>The vector3 value</returns>
	virtual Vector3f GetValueVector3() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of vector3's.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The vector3 array</returns>
	virtual vector<Vector3f> GetValueVector3Array(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a vector4.
	/// </summary>
	/// <returns>The vector4 value</returns>
	virtual Vector4f GetValueVector4() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of vector4's.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>The vector4 array</returns>
	virtual vector<Vector4f> GetValueVector4Array(int32_t count) const = 0;

	/// <summary>
	/// Gets the value of the parameter as a String.
	/// </summary>
	/// <returns>The string value</returns>
	virtual String GetValueString() const = 0;

	/// <summary>
	/// Gets the value of the parameter as a Texture1D.
	/// </summary>
	/// <returns>The texture value</returns>
	virtual shared_ptr<Texture> GetValueTexture1D() const = 0;

	/// <summary>
	/// Gets the value of the parameter as a Texture2D.
	/// </summary>
	/// <returns>The texture value</returns>
	virtual shared_ptr<Texture> GetValueTexture2D() const = 0;

	/// <summary>
	/// Gets the value of the parameter as a Texture3D.
	/// </summary>
	/// <returns>The texture value</returns>
	virtual shared_ptr<Texture> GetValueTexture3D() const = 0;

	/// <summary>
	/// Gets the value of the parameter as a TextureCube.
	/// </summary>
	/// <returns>The texture value</returns>
	virtual shared_ptr<Texture> GetValueTextureCube() const = 0;

	/// <summary>
	/// Gets the value of the parameter as an array of textures.
	/// </summary>
	/// <param name="count">Number of elements to get</param>
	/// <returns>Texture array</returns>
	virtual vector<shared_ptr<Texture> > GetValueTextureArray(int32_t count) const = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Bool value</param>
	virtual void SetValue(bool value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of bools</param>
	virtual void SetValue(const vector<bool>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Single value</param>
	virtual void SetValue(float value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of singles</param>
	virtual void SetValue(const vector<float>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Int value</param>
	virtual void SetValue(int value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of ints</param>
	virtual void SetValue(const vector<int32_t>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Matrix value</param>
	virtual void SetValue(const Matrix4f& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of matrices</param>
	virtual void SetValue(const vector<Matrix4f>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Matrix transpose value</param>
	virtual void SetValueTranspose(const Matrix4f& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of transposed matrices</param>
	virtual void SetValueTranspose(const vector<Matrix4f>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Color value (interpreted as a 4-float vector)</param>
	virtual void SetValue(const ColorRGBA& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of color (interpreted as a 4-float vector)</param>
	virtual void SetValue(const vector<ColorRGBA>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Vector2 value</param>
	virtual void SetValue(const Vector2f& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of Vector2</param>
	virtual void SetValue(const vector<Vector2f>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Vector3 value</param>
	virtual void SetValue(const Vector3f& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of Vector3</param>
	virtual void SetValue(const vector<Vector3f>& values) = 0;

	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Vector4 value</param>
	virtual void SetValue(const Vector4f& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of Vector4</param>
	virtual void SetValue(const vector<Vector4f>& values) = 0;


	/// <summary>
	/// Sets the value of the parameter.
	/// </summary>
	/// <param name="value">Texture value</param>
	virtual void SetValue(const shared_ptr<Texture>& value) = 0;

	/// <summary>
	/// Sets the value of the parameter's elements.
	/// </summary>
	/// <param name="values">Array of textures</param>
	virtual void SetValue(const vector<shared_ptr<Texture> >& values) = 0;

protected:
	EffectParameterType mParameterType;
	EffectParameterClass mParameterClass;
	String mName;
	String mSemantic;
	bool mValid;
};

} // Namespace Render
} // Namespace RcEngine


#endif // EffectParameter_h__
