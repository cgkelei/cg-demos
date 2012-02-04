#include "Exception.h"

namespace RcEngine{
namespace Core{

Exception::Exception( int num, const std::string& desc, const std::string& src )
	: number(num), description(desc), source(src)
{

}

Exception::Exception( int num, const std::string& desc, const std::string& src , const char* type, const char* fileName, long lineNum )
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

const std::string& Exception::GetFullDescription( void ) const
{
	if (fullDesc.empty())
	{
		std::stringstream  desc;

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

const std::string& Exception::GetSource() const
{
	return source;
}

const std::string& Exception::GetFile() const
{
	return file;
}

long Exception::GetLineNumber() const
{
	return line;
}

const std::string& Exception::GetDescription( void ) const
{
	return description;
}

} // Namespace Core
} // Namespace RcEngine
