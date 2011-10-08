#include "Exception.h"
#include <sstream>

namespace Engine
{
	
	Exception::Exception( int num, const String& desc, const String& src )
		: number(num), description(desc), source(src)
	{

	}

	Exception::Exception( int num, const String& desc, const String& src , const char* type, const char* fileName, long lineNum )
		: number(num), description(desc), source(src), file(fileName), line(lineNum)
	{

	}

	Exception::Exception( const Exception& rhs )
		: number(rhs.number), description(rhs.description), source(rhs.source), file(rhs.file), line(rhs.line)
	{

	}

	void Exception::operator=( const Exception& rhs )
	{
		number = rhs.number;
		description = rhs.description;
		source = rhs.source;
		file = rhs.file;
		line = rhs.line;
	}

	const String& Exception::GetFullDescription( void ) const
	{
		if (fullDesc.empty())
		{
			std::ostringstream  desc;

			desc <<  "OGRE EXCEPTION(" << number << ":" << typeName << "): "
				<< description 
				<< " in " << source;

			if( line > 0 )
			{
				desc << " at " << file << " (line " << line << ")";
			}

			fullDesc = desc.str();
		}

		return fullDesc;
	}

	int Exception::GetNumber( void ) const throw()
	{
		return number;
	}

	const String& Exception::GetSource() const
	{
		return source;
	}

	const String& Exception::GetFile() const
	{
		return file;
	}

	long Exception::GetLineNumber() const
	{
		return line;
	}

	const String& Exception::GetDescription( void ) const
	{
		return description;
	}
}