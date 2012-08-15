#include "OpenGLEffectParameter.h"
#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLEffectParameter::OpenGLEffectParameter( CGparameter parameter )
			: mCgParameter(parameter), mNumRows(0), mNumCols(0), mCollection(nullptr)
		{
			if ( mCgParameter && cgIsParameter(mCgParameter) )
			{
				mValid = true;

				const char* name = cgGetParameterName(mCgParameter);
				if ( name )
				{
					mName = name;
				}
				const char* semantic = cgGetParameterSemantic(mCgParameter);
				if ( semantic )
				{
					mSemantic = semantic;
				}

				mCgParameterClass = cgGetParameterClass(mCgParameter);
				mCgType = cgGetParameterType(mCgParameter);
				mCgBaseType = cgGetParameterBaseType( mCgParameter );
				VerifyParameterType();

			}
			else
			{
				mValid = false;
				mName = "InvalidEffectParameter";
			}
		}

		OpenGLEffectParameter::~OpenGLEffectParameter()
		{
			if (mCollection)
			{
				Safe_Delete(mCollection);
			}
		}

		void OpenGLEffectParameter::VerifyParameterType()
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
						mParameterType = EPT_Boolean;
						break;
					case CG_INT:
						mParameterType = EPT_Int;
						break;
					case CG_FLOAT:
						mParameterType = EPT_Float;
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
							if(mNumCols == 2) mParameterType = EPT_Int2;
							else if(mNumCols == 3) mParameterType = EPT_Int3;
							else if(mNumCols == 4) mParameterType = EPT_Int4;
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
							if(mNumCols == 2) mParameterType = EPT_Float2;
							else if(mNumCols == 3) mParameterType = EPT_Float3;
							else if(mNumCols == 4) mParameterType = EPT_Float4;
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

					/*CGenum order = cgGetMatrixParameterOrder(mCgParameter);
					if (order == CG_ROW_MAJOR )
					{
						mColumnMatrix = false;
					}else if (order == CG_COLUMN_MAJOR )
					{
						mColumnMatrix = true;
					}else
					{
						assert(false);
					}*/
									

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
								mParameterType = EPT_Matrix2x2;
							else if(mNumRows == 3 && mNumCols == 3)
								mParameterType = EPT_Matrix3x3;
							else if(mNumRows == 4 && mNumCols == 4)
								mParameterType = EPT_Matrix4x4;
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
					mParameterType = EPT_Sampler;
				}
				break;

			case CG_PARAMETERCLASS_STRUCT:
				{
					mParameterType = EPT_Struct;

					mCollection = new EffectParameterCollection();	
					CGparameter structParam = cgGetFirstStructParameter(mCgParameter);
					while ( structParam != NULL )
					{
						mCollection->Add(new OpenGLEffectParameter( structParam ));
						structParam = cgGetNextParameter( structParam );
					}

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
					//int dimension = cgGetArrayDimension(mCgParameter);
					//if (dimension > 1)
					//{
					//	std::string errString("Only 1d array are supported");
					//	ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
					//		errString, "OpenGLRenderParameter::VerifyParameterType");
					//}

					//mCollection = new EffectParameterCollection;
					//int arraySize = cgGetArraySize(mCgParameter, 0);
					//for (int i = 0; i < arraySize; ++i)
					//{
					//	mCollection->Add(new OpenGLEffectParameter( 
					//		cgGetArrayParameter(mCgParameter, i)));
					//}
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

		EffectParameterCollection* OpenGLEffectParameter::GetElements() const
		{
			return mCollection;
		}

		EffectParameterCollection* OpenGLEffectParameter::StructureMembers() const
		{
			return mCollection;
		}


		bool OpenGLEffectParameter::GetValueBoolean() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType == CG_BOOL )
			{
				int res;
				cgGetParameterValueir(mCgParameter, 1,  &res);
				return res != 0;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLEffectParameter::GetValueBoolean()");
			}
		}

		vector<bool> OpenGLEffectParameter::GetValueBooleanArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueBoolean()");
		}

		float OpenGLEffectParameter::GetValueFloat() const
		{
			if (mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType == CG_FLOAT )
			{
				float res;
				cgGetParameterValuefr(mCgParameter, 1,  &res);
				return res;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLEffectParameter::GetValueFloat()");
			}
		}

		vector<float> OpenGLEffectParameter::GetValueFloatArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueBoolean()");
		}

		int32_t OpenGLEffectParameter::GetValueInt() const
		{
			if (mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType == CG_INT )
			{
				int res;
				cgGetParameterValueir(mCgParameter, 1,  &res);
				return res;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLEffectParameter::GetValueFloat()");
			}
		}

		vector<int32_t> OpenGLEffectParameter::GetValueIntArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueBoolean()");
		}


		Matrix4f OpenGLEffectParameter::GetValueMatrix() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_MATRIX && mCgType ==CG_FLOAT4x4)
			{
				float mat[16];
				assert( mNumCols * mNumRows == 16);
				if (mColumnMatrix)
				{
					cgGetParameterValuefc(mCgParameter, 16, mat);
				}
				else
				{
					cgGetParameterValuefc(mCgParameter, 16, mat);
				}
				return Matrix4f(mat);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueMatrix( )");
			}
		}


		vector<Matrix4f> OpenGLEffectParameter::GetValueMatrixArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueMatrixArray()");
		}

		Matrix4f OpenGLEffectParameter::GetValueMatrixTranspose() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_MATRIX && mCgType ==CG_FLOAT4x4)
			{
				float mat[16];
				assert( mNumCols * mNumRows == 16);
				if (mColumnMatrix)
				{
					cgGetParameterValuefc(mCgParameter, 16, mat);
				}
				else
				{
					cgGetParameterValuefc(mCgParameter, 16, mat);
				}
				
				Matrix4f res(mat);
				res.Transpose();
				return res;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueMatrix( )");
			}
		}

		vector<Matrix4f> OpenGLEffectParameter::GetValueMatrixTransposeArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueMatrixTransposeArray()");
		}

		ColorRGBA OpenGLEffectParameter::GetValueColor() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType == CG_FLOAT4)
			{
				float value[4];
				cgGetParameterValuefr(mCgParameter, 4, value);
				return ColorRGBA(value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueColor()");
			}
		}

		vector<ColorRGBA> OpenGLEffectParameter::GetValueColorArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueColorArray()");
		}

		Vector2f OpenGLEffectParameter::GetValueVector2() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT2)
			{
				float value[2];
				cgGetParameterValuefr(mCgParameter, 2, value);
				return Vector2f(value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueVector2()");
			}
		}

		vector<Vector2f> OpenGLEffectParameter::GetValueVector2Array( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueVector2Array()");
		}

		Vector3f OpenGLEffectParameter::GetValueVector3() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT3)
			{
				float value[3];
				cgGetParameterValuefr(mCgParameter, 3, value);
				return Vector3f(value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueVector2()");
			}
		}

		vector<Vector3f> OpenGLEffectParameter::GetValueVector3Array( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueVector3Array()");
		}

		Vector4f OpenGLEffectParameter::GetValueVector4() const
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT4)
			{
				float value[4];
				cgGetParameterValuefr(mCgParameter, 4, value);
				return Vector4f(value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::GetValueVector4()");
			}
		}

		vector<Vector4f> OpenGLEffectParameter::GetValueVector4Array( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueVector2Array()");
		}

		String OpenGLEffectParameter::GetValueString() const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter String Not Supported in OpenGL Currently",
				"OpenGLEffectParameter::GetValueString()");
		}

		shared_ptr<Texture> OpenGLEffectParameter::GetValueTexture1D() const
		{
			if (!mCachedTexture && mCachedTexture->GetTextureType() == TT_Texture1D )
			{
				return mCachedTexture;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Parameter is not a Texture1D",
					"OpenGLEffectParameter::GetValueTexture1D()");	
			}
		}

		shared_ptr<Texture> OpenGLEffectParameter::GetValueTexture2D() const
		{
			if (!mCachedTexture && mCachedTexture->GetTextureType() == TT_Texture2D )
			{
				return mCachedTexture;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Parameter is not a Texture2D",
					"OpenGLEffectParameter::GetValueTexture2D()");	
			}
		}

		shared_ptr<Texture> OpenGLEffectParameter::GetValueTexture3D() const
		{
			if (!mCachedTexture && mCachedTexture->GetTextureType() == TT_Texture1D )
			{
				return mCachedTexture;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Parameter is not a Texture1D",
					"OpenGLEffectParameter::GetValueTexture3D()");	
			}
		}

		shared_ptr<Texture> OpenGLEffectParameter::GetValueTextureCube() const
		{
			if (!mCachedTexture && mCachedTexture->GetTextureType() == TT_TextureCube )
			{
				return mCachedTexture;
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Parameter is not a Texture1D",
					"OpenGLEffectParameter::GetValueTextureCube()");	
			}
		}

		vector<shared_ptr<Texture> > OpenGLEffectParameter::GetValueTextureArray( int32_t count ) const
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::GetValueVector2Array()");
		}

		void OpenGLEffectParameter::SetValue( bool value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType == CG_BOOL1 )
			{
				cgSetParameter1i(mCgParameter, static_cast<int32_t>(value));
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(value)");
			}
		}

		void OpenGLEffectParameter::SetValue(const vector<bool>& values)
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}


		void OpenGLEffectParameter::SetValue( float value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType ==CG_FLOAT1)
			{
				cgSetParameter1f(mCgParameter, value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(float)");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<float>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( int value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_SCALAR && mCgType ==CG_INT1)
			{
				cgSetParameter1i(mCgParameter, value);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(int)");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<int32_t>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( const Matrix4f& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_MATRIX && mCgType ==CG_FLOAT4x4)
			{
				cgSetMatrixParameterfr(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(const Matrix4f&)");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<Matrix4f>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( const ColorRGBA& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType == CG_FLOAT4)
			{
				cgSetParameter4fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(const ColorRGBA&)");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<ColorRGBA>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( const Vector2f& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT2)
			{
				cgSetParameter2fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector2f& )");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<Vector2f>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( const Vector3f& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT3)
			{
				cgSetParameter3fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue( const Vector3f& )");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<Vector3f>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValue( const Vector4f& value )
		{
			if(mCgParameterClass == CG_PARAMETERCLASS_VECTOR && mCgType ==CG_FLOAT4)
			{
				cgSetParameter4fv(mCgParameter, value());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValue(const Vector4f&)");
			}
		}

		void OpenGLEffectParameter::SetValue( const vector<Vector4f>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

		void OpenGLEffectParameter::SetValueTranspose( const Matrix4f& value )
		{
			Matrix4f transpose = value;
			transpose.Transpose();

			if(mCgParameterClass == CG_PARAMETERCLASS_MATRIX && mCgType ==CG_FLOAT4x4)
			{
				cgSetMatrixParameterfr(mCgParameter, transpose());
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Type Error",
					"OpenGLRenderParameter::SetValueTranspose(const Matrix4f&)");
			}
		}

		void OpenGLEffectParameter::SetValueTranspose( const vector<Matrix4f>& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue( const vector<Matrix4f>&)");
		}

		void OpenGLEffectParameter::SetValue( const shared_ptr<Texture>& value )
		{
			if(!value)
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture cannot be null",
					"OpenGLEffectParameter::SetValue(const shared_ptr<Texture>&)");
			}
			// Cached Texture
			mCachedTexture = value;

			// Set value
			const OpenGLTexture* glTex = static_cast<const OpenGLTexture*>(value.get());
			cgGLSetTextureParameter(mCgParameter, glTex->GetOpenGLTexture());
			cgSetSamplerState(mCgParameter);
		}

		void OpenGLEffectParameter::SetValue( const vector<shared_ptr<Texture> >& values )
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Effect Parameter Array Not Supported Currently",
				"OpenGLEffectParameter::SetValue(const vector<bool>&)");
		}

	}
}