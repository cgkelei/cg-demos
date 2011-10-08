#pragma once
#ifndef _RenderEffectVariable__H
#define _RenderEffectVariable__H


#include "Prerequisites.h"

namespace Engine
{
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


	class EffectVariable
	{
	public:
		EffectVariable();
		virtual ~EffectVariable();

		virtual EffectVariablePtr Clone() = 0;
		
		// EffectScalarVariable
		virtual EffectVariable& operator=(bool const & value);
		virtual EffectVariable& operator=(uint32 const & value);
		virtual EffectVariable& operator=(int32 const & value);
		virtual EffectVariable& operator=(float const & value);	
		virtual EffectVariable& operator=(Vector4f const & value);
		
		// EffectMatrixVariable
		virtual EffectVariable& operator=(Matrix4f const & value);
		
		// EffectVectorVariable 
		virtual EffectVariable& operator=(std::vector<bool> const & value);
		virtual EffectVariable& operator=(std::vector<uint32> const & value);
		virtual EffectVariable& operator=(std::vector<int32> const & value);
		virtual EffectVariable& operator=(std::vector<float> const & value);
		virtual EffectVariable& operator=(std::vector<Vector4f> const & value);
		virtual EffectVariable& operator=(std::vector<Matrix4f> const & value);
		
		// EffectStringVariable
		virtual EffectVariable& operator=(String const & value);

		// EffectSamplerVariable
		virtual EffectVariable& operator=(SampleStatePtr const & value);
		
		// EffectTextureVariable
		virtual EffectVariable& operator=(TexturePtr const & value);

		//EffectConstantBuffer
		virtual EffectVariable& operator=(HardwareBufferPtr const & value);
		
		// EffectShaderVariable
		//virtual EffectVariable& operator=(shader_desc const & value);

		

		virtual void Value(bool& val) const;
		virtual void Value(uint32& val) const;
		virtual void Value(float& val) const;
		virtual void Value(uint32& val) const;
		virtual void Value(Vector4f& val) const;
		virtual void Value(Matrix4f& val) const;
		virtual void Value(SampleStatePtr& val) const;
		virtual void Value(TexturePtr& val) const;
		virtual void Value(HardwareBufferPtr& val) const;
		virtual void Value(String& val) const;
		
		//virtual void Value(shader_desc& val) const;
		virtual void Value(std::vector<bool>& val) const;
		virtual void Value(std::vector<int32>& val) const;
		virtual void Value(std::vector<uint32>& val) const;
		virtual void Value(std::vector<float>& val) const;
		virtual void Value(std::vector<Vector4f>& val) const;
		virtual void Value(std::vector<Matrix4f>& val) const;
	};


	template < typename T >
	class EffectVariableConcrete : public EffectVariable
	{
	public:
		EffectVariablePtr Clone()
		{
			EffectVariablePtr result; // new Ò»¸ö shared_ptr
			T val;
			this->Value(val);
			*result = val;
			return result;
		}

		 EffectVariable& operator=(T const & value)
		 {
			Val = value;
			return *this;
		 }

		 void Value(T& val) const 
		 {
			 val = Val;
		 }

	private:
		T Val;
	};

	
	typedef EffectVariableConcrete<bool> EffectVariableBool;
	typedef EffectVariableConcrete<int32> EffectVariableInt;
	typedef EffectVariableConcrete<uint32> EffectVariableUInt;
	typedef EffectVariableConcrete<float> EffectVariableFloat;
	typedef EffectVariableConcrete<Vector4f> EffectVariableVector4;
	typedef EffectVariableConcrete<Matrix4f> EffectVariableMatrix4;
	typedef EffectVariableConcrete<std::vector<bool> > EffectVariableBoolArray;
	typedef EffectVariableConcrete<std::vector<uint32> > EffectVariableUIntArray;
	typedef EffectVariableConcrete<std::vector<int32> > EffectVariableIntArray;
	typedef EffectVariableConcrete<std::vector<float> > EffectVariableFloatArray;
	typedef EffectVariableConcrete<std::vector<Vector4f> > EffectVariableVector4Array;
	typedef EffectVariableConcrete<std::vector<Matrix4f> > EffectVariableMatrix4Array;
	typedef EffectVariableConcrete<String> EffectVariableString;
	typedef EffectVariableConcrete<SampleStatePtr> EffectVariableSampler;


	class _ApiExport EffectAnnotation
	{
	public:

		uint32 GetType() const { return DataType; }
		String GetName() const { return Name; }

		template <typename T>
		void Value( T& value ) const 
		{
			Var->Value(value);
		}

	private:
		uint32 DataType;
		String Name;
		EffectVariablePtr Var;

	};

	

		
	class EffectParameter
	{
	private:
		EffectParameter(const EffectParameter&);
		EffectParameter& operator= (const EffectParameter&);

	public:
		explicit EffectParameter(RenderEffect& effect);
		virtual ~EffectParameter();

	
		EffectParameterPtr Clone(RenderEffect& effect);

		ShaderVariableType GetParameterType() const
		{
			return VariableType;
		}

		EffectVariablePtr const & GetVar() const
		{
			return Var;
		}

		uint32 GetArraySize() const
		{
			return ArraySize;
		}

		String GetName() const
		{
			return Name;
		}

		String GetSemantic() const
		{
			return Semantic;
		}

		uint32 GetNumAnnotations() const
		{
			return static_cast<uint32>(Annotations->size());
		}

		EffectAnnotationPtr GetAnnotation(uint32 index)
		{
			assert(index < GetNumAnnotations() );
			return Annotations[index];
		}


		virtual EffectParameter& operator=(bool const & value)						{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(uint32 const & value)					{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(int32 const & value)						{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(float const & value)						{  *Var = value; return  &this; }	
		virtual EffectParameter& operator=(Vector4f const & value)					{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(Matrix4f const & value)					{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<bool> const & value)			{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<uint32> const & value)		{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<int32> const & value)		{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<float> const & value)		{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<Vector4f> const & value)		{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(std::vector<Matrix4f> const & value)		{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(String const & value)					{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(SampleStatePtr const & value)			{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(TexturePtr const & value)				{  *Var = value; return  &this; }
		virtual EffectParameter& operator=(HardwareBufferPtr const & value)			{  *Var = value; return  &this; }

		virtual void Value(bool& val) const						{ Var->Value(val); }
		virtual void Value(uint32& val) const					{ Var->Value(val); }
		virtual void Value(float& val) const					{ Var->Value(val); }
		virtual void Value(uint32& val) const					{ Var->Value(val); }
		virtual void Value(Vector4f& val) const					{ Var->Value(val); }
		virtual void Value(Matrix4f& val) const					{ Var->Value(val); }
		virtual void Value(SampleStatePtr& val) const			{ Var->Value(val); }
		virtual void Value(TexturePtr& val) const				{ Var->Value(val); }
		virtual void Value(HardwareBufferPtr& val) const		{ Var->Value(val); }
		virtual void Value(String& val) const					{ Var->Value(val); }
		virtual void Value(std::vector<bool>& val) const		{ Var->Value(val); }
		virtual void Value(std::vector<int32>& val) const		{ Var->Value(val); }
		virtual void Value(std::vector<uint32>& val) const		{ Var->Value(val); }
		virtual void Value(std::vector<float>& val) const		{ Var->Value(val); }
		virtual void Value(std::vector<Vector4f>& val) const	{ Var->Value(val); }
		virtual void Value(std::vector<Matrix4f>& val) const	{ Var->Value(val); }

	protected:
		RenderEffect& effect_;
		
		String Name;
		String Semantic;
		uint32 ArraySize;
		ShaderVariableType VariableType;
		EffectVariablePtr Var;

		std::vector<EffectAnnotationPtr> Annotations;
	};

}

#endif