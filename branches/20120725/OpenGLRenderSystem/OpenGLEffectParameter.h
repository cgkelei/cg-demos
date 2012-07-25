#ifndef OpenGLEffectParameter_h__
#define OpenGLEffectParameter_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include "Graphics/EffectParameter.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLEffectParameter : public EffectParameter 
		{
		public:
			OpenGLEffectParameter(CGparameter parameter);
			~OpenGLEffectParameter();

			EffectParameterCollection* GetElements() const;
			EffectParameterCollection* StructureMembers() const ; 

			bool GetValueBoolean() const;
			vector<bool> GetValueBooleanArray(int32_t count) const;
			float GetValueFloat() const;
			vector<float> GetValueFloatArray(int32_t count) const;
			int32_t GetValueInt() const;
			vector<int32_t> GetValueIntArray(int32_t count) const;
			Matrix4f GetValueMatrix() const;
			vector<Matrix4f> GetValueMatrixArray(int32_t count) const;
			Matrix4f GetValueMatrixTranspose() const;
			vector<Matrix4f> GetValueMatrixTransposeArray(int32_t count) const;
			ColorRGBA GetValueColor() const;
			vector<ColorRGBA> GetValueColorArray(int32_t count) const;
			Vector2f GetValueVector2() const;
			vector<Vector2f> GetValueVector2Array(int32_t count) const;
			Vector3f GetValueVector3() const;
			vector<Vector3f> GetValueVector3Array(int32_t count) const;
			Vector4f GetValueVector4() const;
			vector<Vector4f> GetValueVector4Array(int32_t count) const;
			String GetValueString() const;
			shared_ptr<Texture> GetValueTexture1D() const;
			shared_ptr<Texture> GetValueTexture2D() const;
			shared_ptr<Texture> GetValueTexture3D() const;
			shared_ptr<Texture> GetValueTextureCube() const;
			vector<shared_ptr<Texture> > GetValueTextureArray(int32_t count) const;

			void SetValue(bool value);
			void SetValue(const vector<bool>& values);
			void SetValue(float value);
			void SetValue(const vector<float>& values);
			void SetValue(int value);
			void SetValue(const vector<int32_t>& values);
			void SetValue(const Matrix4f& value);
			void SetValue(const vector<Matrix4f>& values);
			void SetValueTranspose(const Matrix4f& value);
			void SetValueTranspose(const vector<Matrix4f>& values);
			void SetValue(const ColorRGBA& value);
			void SetValue(const vector<ColorRGBA>& values);
			void SetValue(const Vector2f& value);
			void SetValue(const vector<Vector2f>& values);
			void SetValue(const Vector3f& value);
			void SetValue(const vector<Vector3f>& values);
			void SetValue(const Vector4f& value);
			void SetValue(const vector<Vector4f>& values);
			void SetValue(const shared_ptr<Texture>& value);
			void SetValue(const vector<shared_ptr<Texture> >& values);

		private:
			void VerifyParameterType();

		private:
			CGparameter mCgParameter;
			CGparameterclass mCgParameterClass;
			CGtype mCgType;
			CGtype mCgBaseType;
			int32_t mNumRows, mNumCols;
			bool mColumnMatrix;

			shared_ptr<Texture> mCachedTexture; 
			EffectParameterCollection* mCollection;

		};
	}
}



#endif // OpenGLEffectParameter_h__
