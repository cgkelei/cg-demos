#ifndef OpenGLRenderParameter_h__
#define OpenGLRenderParameter_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/RenderPatameter.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		class _OpenGLExport OpenGLRenderParameter : public RenderParameter
		{
		public:
			OpenGLRenderParameter(CGparameter parameter);
			~OpenGLRenderParameter();

			void SetValue(const bool& value);
			void SetValue(const ColorRGBA& value);

			void SetValue(const int& value);
			void SetValue(const Vector<int, 2>& value);
			void SetValue(const Vector<int, 3>& value);
			void SetValue(const Vector<int, 4>& value);

			void SetValue(const unsigned int& value);
			void SetValue(const Vector<unsigned int, 2>& value);
			void SetValue(const Vector<unsigned int, 3>& value);
			void SetValue(const Vector<unsigned int, 4>& value);

			void SetValue(const float& value);
			void SetValue(const Vector<float, 2>& value);
			void SetValue(const Vector<float, 3>& value);
			void SetValue(const Vector<float, 4>& value);

			void SetValue(const Matrix4<float>& value);

			void SetValue(const Texture* value);

		private:
			void VerifyParameterType();


		private:
			CGparameter mCgParameter;
			CGparameterclass mCgParameterClass;
			CGtype mCgType;
			CGtype mCgBaseType;
			int mNumRows, mNumCols;

		};
	}
}


#endif // OpenGLRenderParameter_h__
