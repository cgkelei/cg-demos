#include "RenderEffectVariable.h"

namespace Engine
{

	EffectVariable::EffectVariable()
	{

	}


	EffectVariable::~EffectVariable()
	{

	}

	EffectVariable& EffectVariable::operator=( bool const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( uint32 const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( int32 const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( float const & value)	
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( Vector4f const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( Matrix4f const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( std::vector<bool> const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=(std::vector<uint32> const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( std::vector<int32> const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( std::vector<float> const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( std::vector<Vector4f> const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=(std::vector<Matrix4f> const & value)
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( String const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( SampleStatePtr const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=( TexturePtr const & value )
	{
		assert(false);
		return *this;
	}

	EffectVariable& EffectVariable::operator=(HardwareBufferPtr const & value)
	{
		assert(false);
		return *this;
	}

	void EffectVariable::Value( bool& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( uint32& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( float& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( uint32& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( Vector4f& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( Matrix4f& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( SampleStatePtr& val) const
	{
		assert(false);
	}

	void EffectVariable::Value( String& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value( std::vector<bool>& val ) const
	{
		assert(false);
	}

	void EffectVariable::Value(std::vector<int32>& val) const
	{
		assert(false);
	}

	void EffectVariable::Value(std::vector<uint32>& val) const
	{
		assert(false);
	}
	
	void EffectVariable::Value(std::vector<float>& val) const
	{
		assert(false);

	}

	void EffectVariable::Value(std::vector<Vector4f>& val) const
	{
		assert(false);
	}

	void EffectVariable::Value(std::vector<Matrix4f>& val) const
	{
		assert(false);
	}

	void EffectVariable::Value(TexturePtr& val) const
	{
		assert(false);
	}

	void EffectVariable::Value(HardwareBufferPtr& val) const
	{
		assert(false);
	}

}