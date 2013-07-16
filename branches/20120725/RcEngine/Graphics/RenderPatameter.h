#ifndef EffectPatameter_h__
#define EffectPatameter_h__

#include "Core/Prerequisites.h"
#include "Math/MathUtil.h"
#include "Math/ColorRGBA.h"

namespace RcEngine {
	namespace Render {

		using namespace Math;

		class _ApiExport RenderParameter 
		{
		public:
			enum ParamType
			{
				PT_Boolean = 0,
				PT_Float,
				PT_Float2,
				PT_Float3,
				PT_Float4,
				PT_Int,
				PT_Int2,
				PT_Int3,
				PT_Int4,
				PT_Uint,
				PT_Uint2,
				PT_Uint3,
				PT_Uint4,
				PT_Matrix2x2,
				PT_Matrix3x3,
				PT_Matrix4x4,
				PT_Sampler,
				PT_Struct_Buffer,
				PT_Count
			};

		public:
			RenderParameter();
			virtual ~RenderParameter();

			ParamType GetParameterType() const;
			bool HasSemantic() const;
			const std::string& GetParameterName() const;
			const std::string& GetParameterSemantic() const;

			virtual void SetValue(const bool& value);

			virtual void SetValue(const int32_t& value);
			virtual void SetValue(const ColorRGBA& value);
			virtual void SetValue(const Vector<int32_t, 2>& value);
			virtual void SetValue(const Vector<int32_t, 3>& value);
			virtual void SetValue(const Vector<int32_t, 4>& value);

			virtual void SetValue(const uint32_t& value);
			virtual void SetValue(const Vector<uint32_t, 2>& value);
			virtual void SetValue(const Vector<uint32_t, 3>& value);
			virtual void SetValue(const Vector<uint32_t, 4>& value);

			virtual void SetValue(const float& value);
			virtual void SetValue(const Vector<float, 2>& value);
			virtual void SetValue(const Vector<float, 3>& value);
			virtual void SetValue(const Vector<float, 4>& value);

			virtual void SetValue(const Matrix4<float>& value);

			virtual void SetValue(const shared_ptr<Texture>& value);

		protected:
			std::string         mParameterName;
			std::string         mParameterSemantic;
			ParamType			mParameterType;
			bool                mIsValid;

		public:
			static String TypeNames[RenderParameter::PT_Count];
		};



	} // RenderSystem
} // RcEngine



#endif // EffectPatameter_h__