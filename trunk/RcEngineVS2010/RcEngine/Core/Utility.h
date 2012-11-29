#pragma once
#ifndef _Utility__H
#define _Utility__H

#include <Core/Prerequisites.h>

// Convert Build-In type to String
template<typename T>
inline std::string ToString(const T& value)
{
	std::stringstream str;
	str << value;
	return str.str();
}

// Convert string to other number type
template<typename Target, typename Source>
inline typename Target LexicalCast( const Source& value )
{
	Target re;
	std::stringstream str;
	str << value;
	str >> re;
	return re;
}

std::wstring& Convert(std::wstring& dest, std::string const & src);
uint32_t FloatToUint32(float v);
float Uint32ToFloat(uint32_t v);


#endif // _Utility__H