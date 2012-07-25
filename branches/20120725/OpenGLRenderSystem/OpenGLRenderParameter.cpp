#include "OpenGLRenderParameter.h"
#include "OpenGLTexture.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		//////////////////////////////////////////////////////////////////////////
		OpenGLRenderParameter::OpenGLRenderParameter( CGparameter parameter )
			: mCgParameter(parameter), mNumRows(0), mNumCols(0)
		{
			if ( mCgParameter != NULL && cgIsParameter(mCgParameter) )
			{
				mIsValid = true;

				const char* name = cgGetParameterName(mCgParameter);
				if ( name != NULL )
				{
					mParameterName = name;
				}
				const char* semantic = cgGetParameterSemantic(mCgParameter);
				if ( semantic != NULL)
				{
					mParameterSemantic = semantic;

				}

				mCgParameterClass = cgGetParameterClass(mCgParameter);
				mCgType = cgGetParameterType(mCgParameter);
				mCgBaseType = cgGetParameterBaseType( mCgParameter );
				VerifyParameterType();

			}
			else
			{
				mParameterName = "InvalidEffectParameter";
			}
		}

		OpenGLRenderParameter::~OpenGLRenderParameter()
		{

		}

		void OpenGLRenderParameter::VerifyParameterType()
		{
			switch(mCgParameterClass)
			{
			case CG_PARAMETERCLASS_SCALAR:
				{
					mNumRows = cgGetParameterRows( mCgParameter );
					mNumCols = cgGetParameterColumns( mCgParameter );
					assert(mNumRows = 1 && mNumCols == 1);
					switch ( mCgBaseType )
					{
					case CG_BOOL:
						mParameterType = PT_Boolean;
						break;
					case CG_INT:
						mParameterType = PT_Int;
						break;
					case CG_FLOAT:
						mParameterType = PT_Float;
						break;
					case CG_DOUBLE:
						ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
							"double parameter are not support in shader", "OpenGLRenderParameter::VerifyParameterType");
						break;
					default:
						std::string errString("Unsupported parameter base type");
						errString += cgGetTypeString(mCgBaseType);
						ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
							errString, "OpenGLRenderParameter::VerifyParameterType");
					}
				}
				break;

			case CG_PARAMETERCLASS_VECTOR:
				{
					mNumRows = cgGetParameterRows( mCgParameter );
					mNumCols = cgGetParameterColumns( mCgParameter );
					assert(mNumRows = 1 && mNumCols > 1);

					switch ( mCgBaseType )
					{
					case CG_BOOL:
						{
							std::string errString("Boolean Vector are not supported parameter");
							ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
								errString, "OpenGLRenderParameter::VerifyParameterType");
						}
						break;

					case CG_INT:
						{
							if(mNumCols == 2) mParameterType = PT_Int2;
							else if(mNumCols == 3) mParameterType = PT_Int3;
							else if(mNumCols == 4) mParameterType = PT_Int4;
							else 
							{
								std::string errString("Unsupported parameter base type");
								errString += cgGetTypeString(mCgBaseType);
								ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
									errString, "OpenGLRenderParameter::VerifyParameterType");
							}
						}
						break;

					case CG_FLOAT:
						{
							if(mNumCols == 2) mParameterType = PT_Float2;
							else if(mNumCols == 3) mParameterType = PT_Float3;
							else if(mNumCols == 4) mParameterType = PT_Float4;
							else 
							{
								std::string errString("Unsupported parameter base type");
								errString += cgGetTypeString(mCgBaseType);
								ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
									errString, "OpenGLRenderParameter::VerifyParameterType");
							}
						}
						break;

					case CG_DOUBLE:
						{
							ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
								"double parameter are not support in shader", "OpenGLRenderParameter::VerifyParameterType");
						}
						break;
					default:
						{
							std::string errString("Unsupported parameter base type");
							errString += cgGetTypeString(mCgBaseType);
							ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
								errString, "OpenGLRenderParameter::VerifyParameterType");
						}

					}
				}
				break;

			case CG_PARAMETERCLASS_MATRIX:
				{
					mNumRows = cgGetParameterRows( mCgParameter );
					mNumCols = cgGetParameterColumns( mCgParameter );

					switch ( mCgBaseType )
					{
					case CG_BOOL:
					case CG_INT:
					case CG_DOUBLE:
						{
							std::string errString("Only Float Matrix are supported");
							ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
								errString, "OpenGLRenderParameter::VerifyParameterType");
						}
						break;

					case CG_FLOAT:
						{
							if(mNumRows == 2 && mNumCols == 2)
								mParameterType = PT_Matrix2x2;
							else if(mNumRows == 3 && mNumCols == 3)
								mParameterType = PT_Matrix3x3;
							else if(mNumRows == 4 && mNumCols == 4)
								mParameterType = PT_Matrix4x4;
							else
							{
								std::string errString("Only 2x2, 3x3, 4x4 float matrix are supported");
								ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
									errString, "OpenGLRenderParameter::VerifyParameterType");
							}
						}
						break;

					default:
						{
							std::string errString("Unsupported parameter base type");
							errString += cgGetTypeString(mCgBaseType);
							ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
								errString, "OpenGLRenderParameter::VerifyParameterType");
						}
					}
				}
				break;
			case CG_PARAMETERCLASS_SAMPLER:
				{
					mParameterType = PT_Sampler;
				}
				break;

			case CG_PARAMETERCLASS_STRUCT:
				{
					mParameterType = PT_Struct_Buffer;
					//if(mParameterSemantic == "CBuffer")
					//{
					//	CGparameter parameter = cgGetFirstStructParameter( mCgParameter );
					//	while ( parameter != NULL )
					//	{
					//		// Create a new parameter and add it to the internal parameter list
					//		OpenGLRenderParameter* pParameter = new OpenGLRenderParameter( m_cgContext, parameter );
					//		m_NestedParameters.push_back( pParameter );
					//		m_NestedParametersByName.insert( ParameterMap::value_type( pParameter->GetParameterName(), pParameter ) );

					//		parameter = cgGetNextParameter(parameter);
					//	}
					//}
				}
				break;
			case CG_PARAMETERCLASS_ARRAY:
				{
	
				}
				break;
			case CG_PARAMETERCLASS_OBJECT:
			default:
				{
					std::string errString("Unsupported parameter base type:");
					errString += cgGetTypeString(mCgBaseType);
					ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
						errString, "OpenGLRenderParameter::VerifyParameterType");
				}

			}
		}

		void OpenGLRenderParameter::SetValue( const bool& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType == CG_BOOL1 )
			{
				cgSetParameter1i(mCgParameter, static_cast<int32_t>(value));
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const bool& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const ColorRGBA& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType == CG_FLOAT4)
			{
				cgSetParameter4fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<float, 4>& value )");
			}
		}

		void OpenGLRenderParameter::SetValue( const int32_t& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType ==CG_INT1)
			{
				cgSetParameter1i(mCgParameter, value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const int32& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<int32_t, 2>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_INT2)
			{
				cgSetParameter2iv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<int32, 2>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<int32_t, 3>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_INT3)
			{
				cgSetParameter3iv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<int32, 3>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<int32_t, 4>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_INT4)
			{
				cgSetParameter4iv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<int32, 4>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const uint32_t& value )
		{

		}

		void OpenGLRenderParameter::SetValue( const Vector<uint32_t, 2>& value )
		{

		}

		void OpenGLRenderParameter::SetValue( const Vector<uint32_t, 3>& value )
		{

		}

		void OpenGLRenderParameter::SetValue( const Vector<uint32_t, 4>& value )
		{

		}

		void OpenGLRenderParameter::SetValue( const float& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType ==CG_FLOAT1)
			{
				cgSetParameter1f(mCgParameter, value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const float& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<float, 2>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT2)
			{
				cgSetParameter2fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<float, 2>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<float, 3>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT3)
			{
				cgSetParameter3fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<float, 3>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Vector<float, 4>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType == CG_FLOAT4)
			{
				cgSetParameter4fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<float, 4>& value )");
			}

		}

		void OpenGLRenderParameter::SetValue( const Matrix4<float>& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_MATRIX && mCgType ==CG_FLOAT4x4)
			{
				cgSetMatrixParameterfr(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector<int32, 4>& value )");
			}
		}

		void OpenGLRenderParameter::SetValue( const shared_ptr<Texture>& value)
		{
			const OpenGLTexture* glTex = static_cast<const OpenGLTexture*>(value.get());

			cgGLSetTextureParameter(mCgParameter, glTex->GetOpenGLTexture());
			cgSetSamplerState(mCgParameter);
		}



	}
}
