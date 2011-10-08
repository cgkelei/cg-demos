#include "RenderEffect.h"

namespace Engine
{
	RenderEffectPtr RenderEffect::Null;

	// Functor to find variable by name
	/*struct EffectConstantBufferEqPred : public std::unary_function<std::string, bool> {
		EffectConstantBufferEqPred(std::string name_) : name(name_) { }
		std::string name;
		bool operator()(const std::tr1::shared_ptr<EffectConstantBuffer>& buffer) {return buffer->getDesc().name == name;}
	};*/

	struct EffectVarNameEqPred : public std::unary_function<std::string, bool> {
		EffectVarNameEqPred(std::string name_) : name(name_) { }
		std::string name;
		bool operator()(const EffectVariable& buffer) { return buffer->GetDesc().Name == name;}
	};

	struct EffectVarSemanticEqPred : public std::unary_function<std::string, bool> {
		EffectVarNameEqPred(std::string semantic_) : semantic(semantic_) { }
		std::string semantic;
		bool operator()(const EffectVariable& buffer) { return buffer->GetDesc().Semantic == semantic;}
	};

	
	Engine::uint32 RenderEffect::GetNumEffectVariables( void ) const
	{
		return static_cast<uint32>(mEffectVariables.size());
	}


	Engine::EffectVariable RenderEffect::GetVariableByName( const String& name ) const
	{
		// Find the requested variable
		std::vector<EffectVariable>::const_iterator varIt =
			std::find_if(mEffectVariables.begin(), mEffectVariables.end(), EffectVarNameEqPred(name));
		if (varIt != mEffectVariables.end())
			return *varIt;

		// return std::tr1::shared_ptr<EffectVariable>();
	}

	
	Engine::EffectVariable RenderEffect::GetVariableByIndex( uint32 index ) const
	{
		assert( index  < GetNumEffectVariables() );
		return mEffectVariables[index];
	}

	Engine::EffectVariable RenderEffect::GetVariableBySemantic( const String& semantic ) const
	{	
		// Find the requested variable
		std::vector<EffectVariable>::const_iterator varIt =
			std::find_if(mEffectVariables.begin(), mEffectVariables.end(), EffectVarSemanticEqPred(semantic));
		if (varIt != mEffectVariables.end())
			return *varIt;
	}

	Engine::uint32 RenderEffect::GetNumTechniques( void ) const
	{
		return static_cast<uint32>(mTechniques.size());
	}

	
}