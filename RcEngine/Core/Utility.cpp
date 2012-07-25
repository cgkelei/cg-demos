#include "Utility.h"

std::wstring& Convert( std::wstring& dest, std::string const & src )
{
	int32_t const wcs_len = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int32_t>(src.size()), NULL, 0);
	std::vector<wchar_t> tmp(wcs_len + 1);
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int32_t>(src.size()), &tmp[0], wcs_len);
	dest.assign(tmp.begin(), tmp.end() - 1);
	return dest;
}

uint32_t FloatToUint32( float v )
{
	union FNU
	{
		float f;
		uint32_t u;
	} fnu;
	fnu.f = v;
	return static_cast<uint32_t>(fnu.f);
}

float Uint32ToFloat( uint32_t v )
{
	union FNU
	{
		float f;
		uint32_t u;
	} fnu;
	fnu.u = v;
	return static_cast<float>(fnu.f);
}

