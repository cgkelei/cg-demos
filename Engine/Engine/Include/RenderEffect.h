#pragma once
#ifndef _RenderEffect__H
#define _RenderEffect__H

#include "Prerequisites.h"
#include "Vector.h"
#include "Matrix.h"

namespace Engine
{

	class _ApiExport RenderShaderFunc
	{
	public:
		
		uint32 GetVersion() const    { return mVersion; }
		const String& GetStr() const { return mStr; }
		Shader::ShaderType GetType() const  { return mType; }

	private:
		Shader::ShaderType mType;
		uint32 mVersion;
		String mStr;

	};

	class _ApiExport RenderEffect
	{
	public:
		RenderEffect();
		virtual ~RenderEffect();
		
		uint32		   GetNumEffectVariables( void ) const;
		EffectVariable GetVariableByName( const String& name ) const;
		EffectVariable GetVariableByIndex( uint32 index ) const;
		EffectVariable GetVariableBySemantic(const String& semantic ) const;


		uint32 GetNumTechniques (void) const;
		RenderTechniquePtr GetTechniqueByName( const String& techniqueName ) const;
		RenderTechniquePtr GetTechniqueByIndex (uint32 techniqueIndex) const;

		uint32 GetNumShaders() const;
		RenderShaderFunc ShaderByIndex(uint32 index) const;
	
	protected:

		typedef std::pair<String, std::vector<uint32> > CBufferGroup;

#pragma warning(disable : 4251)

		std::vector<EffectParameterPtr>		    mEffectVariables;
		std::vector<CBufferGroup>				mConstantBuffers;
		std::vector<RenderTechniquePtr>         mTechniques;
		std::vector<RenderShaderFunc>		    mShaders;

#pragma warning(default : 4251)	

	public:
		static RenderEffectPtr Null;

	};
}


#endif