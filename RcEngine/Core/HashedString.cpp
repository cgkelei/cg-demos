#include <Core/HashedString.h>

namespace RcEngine {

HashedString::HashedString() 
	: mHash(0)
{

}

HashedString::HashedString( const HashedString& rhs ) 
	: mHash(rhs.mHash)
{

}

HashedString::HashedString( const char* str ) 
	: mHash(CalculateHash(str))
{

}

HashedString::HashedString( const String& str ) 
	: mHash(CalculateHash(str.c_str()))
{

}

uint32_t HashedString::CalculateHash( const String& str )
{
	uint32_t hash = 0;

	for (char c : str)
		hash = c + (hash << 6) + (hash << 16) - hash;  // SDBM Hash

	return hash;
}

String HashedString::ToString() const
{
	char tempBuffer[128];
	sprintf(tempBuffer, "%08X", mHash);
	return String(tempBuffer);
}

}