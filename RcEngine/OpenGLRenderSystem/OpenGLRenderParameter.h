#ifndef OpenGLRenderParameter_h__
#define OpenGLRenderParameter_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/RenderPatameter.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLRenderParameter : public RenderParameter
		{
		public:
			OpenGLRenderParameter(CGparameter parameter);
			~OpenGLRenderParameter();

			void SetValue(const bool& value);
			void SetValue(const ColorRGBA& value);

			void SetValue(const int32_t& value);
			void SetValue(const Vector<int32_t, 2>& value);
			void SetValue(const Vector<int32_t, 3>& value);
			void SetValue(const Vector<int32_t, 4>& value);

			void SetValue(const uint32_t& value);
			void SetValue(const Vector<uint32_t, 2>& value);
			void SetValue(const Vector<uint32_t, 3>& value);
			void SetValue(const Vector<uint32_t, 4>& value);

			void SetValue(const float& value);
			void SetValue(const Vector<float, 2>& value);
			void SetValue(const Vector<float, 3>& value);
			void SetValue(const Vector<float, 4>& value);

			void SetValue(const Matrix4<float>& value);

			void SetValue(const shared_ptr<Texture>& value);

		private:
			void VerifyParameterType();


		private:
			CGparameter mCgParameter;
			CGparameterclass mCgParameterClass;
			CGtype mCgType;
			CGtype mCgBaseType;
			int32_t mNumRows, mNumCols;

		};
	}
}


#endif // OpenGLRenderParameter_h__
