#ifndef StringHash_h__
#define StringHash_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport HashedString
{
public:
	HashedString();
	HashedString(const char* str);
	HashedString(const String& str);
	HashedString(const HashedString& rhs);

	inline bool operator == (const HashedString& rhs) const { return mHash == rhs.mHash; }
	inline bool operator != (const HashedString& rhs) const { return mHash != rhs.mHash; }

	// Used for std::map
	inline bool operator< (const HashedString& rhs) const { return mHash < rhs.mHash; }
	inline bool operator> (const HashedString& rhs) const { return mHash > rhs.mHash; }

	inline uint32_t ToHash() const { return mHash; }

	String ToString() const;

public:
	static uint32_t CalculateHash(const String& str);

private:
	uint32_t mHash;
};

}

// For std::unordered_map hash
namespace std
{
	template<>
	struct hash<RcEngine::HashedString>
	{
	public:
		std::size_t operator()(const RcEngine::HashedString& k) const 
		{
			return k.ToHash();
		}
	};
}


#endif // StringHash_h__
