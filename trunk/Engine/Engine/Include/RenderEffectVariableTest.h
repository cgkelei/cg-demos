#pragma once
#ifndef _RenderEffectVariable__H
#define _RenderEffectVariable__H

#include "Prerequisites.h"

namespace Engine
{
	enum ShaderVariableClass
	{
		SVC_Scalar,
		SVC_Vector,
		SVC_Matrix_Columns,
		SVC_Matrix_Rows,
		SVC_Object,
		SVC_Struct
	};

	enum ShaderVariableType
	{	
		SVT_Bool = 0,
		SVT_String,
		
		SVT_Int,
		SVT_Int2,
		SVT_Int3,
		SVT_Int4,

		SVT_Uint,
		SVT_Uint2,
		SVT_Uint3,
		SVT_Uint4,

		SVT_Float,
		SVT_Float2,
		SVT_Float3,
		SVT_Float4,
		
		SVT_Float2x2,
		SVT_Float2x3,
		SVT_Float2x4,
		
		SVT_Float3x2,
		SVT_Float3x3,
		SVT_Float3x4,

		SVT_Float4x2,
		SVT_Float4x3,
		SVT_Float4x4,

		// Texture Type
		SVT_Texture1D,
		SVT_Texture2D,
		SVT_Texture3D,
		SVT_TextureCube,
		SVT_Texture1DArray,
		SVT_Texture2DArray,
		SVT_Texture3DArray,
		SVT_TextureCubeArray,

		// Sampler Type 
		SVT_Sampler,

		// Shader Type
		SVT_Shader,
			
		// The variable is a constant buffer.
		SVT_Constant_Buffer,
		
		// The variable is a texture buffer.
		SVT_Texture_Buffer, 

		// The variable is a buffer.
		SVT_Buffer, 

		// The variable is a read-and-write buffer.
		SVT_Read_Write_Buffer,

		// The variable is a structured buffer.
		SVT_Structed_Buffer,
		
		// The variable is a read-and-write structured buffer.
		SVT_Read_Write_Structed_Buffer,
			
		
		//The variable is a byte-address buffer.
		SVT_ByteAddress_Buffer,

		//The variable is a read-and-write byte-address buffer.
		SVT_Read_Write_ByteAddress_Buffer,
	};



	struct _ApiExport EffectTypeDesc 
	{
		explicit EffectTypeDesc(ShaderVariableClass object = SVC_Scalar, ShaderVariableType type = SVT_Float,
			unsigned int rows = 0, unsigned int columns = 0, unsigned int elements = 0, using int members = 0) 
			: Class(object), Type(type), Rows(rows), Columns(columns), Elements(elements), Members(members) { }

		ShaderVariableClass  Class;
		ShaderVariableType   Type;

		// Number of rows in this type (0 if not a numeric primitive).
		unsigned int         Rows;

		// Number of columns in this type (0 if not a numeric primitive).
		unsigned int         Columns;

		unsigned int         Elements;

		unsigned int		 Members;
	};
		
	struct _ApiExport EffectVariableDesc
	{
		explicit EffectVariableDesc(String name_ = "", uint32 offset_ = 0, )
			: name(name_), offset(offset_) { }

		String	 Name;
		String	 Semantic;
		uint32   Flags;
		uint32   Annotations;
		uint32   BufferOffset;
	};

	struct EffectVariableSignature {
		explicit EffectVariableSignature(const EffectVariableDesc& varDesc_ = EffectVariableDesc(), const EffectTypeDesc& typeDesc_ = EffectTypeDesc())
			: VarDesc(varDesc_), TypeDesc(typeDesc_) { }

		EffectVariableDesc VarDesc;
		EffectTypeDesc     TypeDesc;
	};


	class _ApiExport EffectVariable
	{
	public:
		EffectVariable();
		virtual ~EffectVariable();

		EffectVariableDesc GetDesc() const;
		EffectTypeDesc     GetType() const;

	protected:
		EffectVariableDesc  Desc;
		EffectTypeDesc      TypeDesc;
		
	};
	
	class _ApiExport EffectConstantBuffer : public EffectVariable {
	public:
		EffectConstantBuffer((const EffectVariableDesc& varDesc, const EffectTypeDesc& typeDesc, HardwareBufferPtr buffer);
		virtual ~EffectConstantBuffer();
		HardwareBufferPtr GetBuffer() const = 0;
		void SetBuffer(HardwareBufferPtr newBuffer) = 0;

	protected:
		HardwareBufferPtr Buffer;
	};

	typename EffectConstantBuffer* EffectConstantBufferPtr;


	class _ApiExport EffectScalarVariable : public EffectVariable
	{
	public:
		EffectScalarVariable((EffectVariableDesc desc, EffectVariableDesc typeDesc, EffectConstantBufferPtr cbuffer);
		virtual~ EffectScalarVariable();	

		// GetBool Get a boolean variable.
		void GetBool(bool* value) const;
		void GetFloat(float* value) const;
		void GetFloatArray(float* value, uint32 offset, uint32 count) const;
		void GetInt(int* value) const;
		void GetIntArray(int* value, uint32 offset, uint32 count) const;
		
		void SetBool( bool value ) const;
		void SetFloat(float value) const;
		void SetFloatArray(float* value, uint32 offset, uint32 count) const;
		void SetInt(int value) const ;
		void SetIntArray(int* value, uint32 offset, uint32 count) const;

	protected:
		EffectConstantBufferPtr Buffer;

	};
	
	class _ApiExport EffectMatrixVariable : public EffectVariable
	{
	public:
		EffectMatrixVariable((EffectVariableDesc desc, EffectVariableDesc typeDesc, EffectConstantBufferPtr cbuffer);
		virtual ~EffectMatrixVariable();

	
		void GetMatrix(float *pData);
		void GetMatrixArray(float *pData, uint32 Offset, uint32 Count );
		void GetMatrixTranspose(float *pData );
		void GetMatrixTransposeArray(float *pData, uint32 Offset, uint32 Count );
		
		void SetMatrix(float *pData);
		void SetMatrixArray ( float *pData, uint32 Offset, uint32 Count );
		void SetMatrixTranspose( float *pData );
		void SetMatrixTransposeArray ( float *pData, uint32 Offset, uint32 Count );

	protected:
		EffectConstantBufferPtr Buffer;
	};
	
	class _ApiExport EffectVectorVariable : public EffectVariable
	{

	};

	class _ApiExport EffectShaderResourceVariable  : public EffectVariable
	{

	};


	class _ApiExport EffectAnnotation
	{
	public:

		uint32 GetType() const { return Type; }
		String GetName() const { return Name; }

		template< typename T >
		void Value( T& value)
		{

		}

	private:
		uint32 Type;
		String Name;
		EffectVariablePtr Var;
	};
}


#endif