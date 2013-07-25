#pragma once
#ifndef _Utility__H
#define _Utility__H

#include <Core/Prerequisites.h>
#include <type_traits>

namespace RcEngine {

// Convert Build-In type to String
template <typename T>
inline std::string ToString(const T& value)
{
	std::stringstream str;
	str << value;
	return str.str();
}

// Convert string to other number type
template <typename Target, typename Source>
inline typename Target LexicalCast( const Source& value )
{
	Target re;
	std::stringstream str;
	str << value;
	str >> re;
	return re;
}

template <typename T, unsigned int Size>
inline T SwapBytes(T value)
{
	union {
		T value;
		char bytes[Size];
	} in, out;

	in.value = value;
	
	for (unsigned int i = 0; i < Size/2; ++i)
	{
		out.bytes[i] = in.bytes[Size-1-i];
		out.bytes[Size-1-i] = in.bytes[i];
	}

	return out.value;
}

template <typename EndianFrom, typename EndianTo, typename T>
inline T EndianSwapBytes(T value)
{
	static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
	static_assert(std::is_arithmetic<T>::value);

	if (EndianFrom == EndianTo)
		return value;
	return SwapBytes<T, sizeof(T)>(value);
}

std::wstring& Convert(std::wstring& dest, std::string const & src);
uint32_t FloatToUint32(float v);
float Uint32ToFloat(uint32_t v);

}

#endif // _Utility__H