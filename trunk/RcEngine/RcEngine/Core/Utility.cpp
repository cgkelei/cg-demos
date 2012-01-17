#include "Utility.h"

std::wstring& Convert( std::wstring& dest, std::string const & src )
{
	int const wcs_len = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int>(src.size()), NULL, 0);
	std::vector<wchar_t> tmp(wcs_len + 1);
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), static_cast<int>(src.size()), &tmp[0], wcs_len);
	dest.assign(tmp.begin(), tmp.end() - 1);
	return dest;
}

uint32 FloatToUint32( float v )
{
	union FNU
	{
		float f;
		uint32 u;
	} fnu;
	fnu.f = v;
	return static_cast<uint32>(fnu.f);
}

float Uint32ToFloat( uint32 v )
{
	union FNU
	{
		float f;
		uint32 u;
	} fnu;
	fnu.u = v;
	return static_cast<float>(fnu.f);
}

