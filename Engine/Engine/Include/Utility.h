#pragma once
#ifndef _Utility__H
#define _Utility__H

#include "Prerequisites.h"

namespace Engine
{
	// Convert Build-In type to String
	template<typename T>
	inline String ToString(const T& value)
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
 
	inline uint32 FloatToUint32(float v)
	{
		union FNU
		{
			float f;
			uint32 u;
		} fnu;
		fnu.f = v;
		return static_cast<uint32>(fnu.f);
	}


	inline float Uint32ToFloat(uint32 v)
	{
		union FNU
		{
			float f;
			uint32 u;
		} fnu;
		fnu.f = v;
		return static_cast<float>(fnu.f);
	}

}


#endif // _Utility__H