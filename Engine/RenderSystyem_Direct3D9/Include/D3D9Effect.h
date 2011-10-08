#pragma once
#ifndef _D3D9Effect__H
#define _D3D9Effect__H

#include "D3D9Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class D3D9Effect : public RenderEffect
		{
		public:
			D3D9Effect();
			~D3D9Effect();

			virtual bool SetValue( const String& paramName, void* pValue, uint32 uiSize );

			virtual bool SetBool( const String& paramName, bool value) = 0;
			virtual bool SetBoolArray( const String& paramName, bool* value, uint32 count);

			virtual bool SetInt( const String& paramName, uint32 value) = 0;
			virtual bool SetIntArray( const String& paramName, int* value, uint32 count);

			virtual bool SetFloat( const String& paramName, float value) = 0;
			virtual bool SetFloatArray( const String& paramName, float* value, uint32 count);

			virtual bool SetVector( const String& paramName,  const Vector4f& value);
			virtual bool SetVectorArray( const String& paramName, const Vector4f* value, uint32 count);

			virtual bool SetMatrix( const String& paramName, const Matrix4f& value );
			virtual bool SetMatrixArray( const String& paramName, const Matrix4f* value, uint32 count );

			virtual bool SetTexture( const String& paramName, Texture* pTexture );

			virtual bool SetTechnique( const String& techName );

			virtual bool Begin( uint32 * passNum );

			virtual bool BeginPass( uint32 pass );
			virtual bool EndPass();

			virtual bool End();


			D3D9Technique* GetCurrentTechnique();
			void Set

		private:

			ID3DXEffect* mpEffect;
	

		};
	}
}


#endif