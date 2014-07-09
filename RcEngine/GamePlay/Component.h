#ifndef Component_h__
#define Component_h__

#include <Core/Prerequisites.h>
#include <Core/HashedString.h>

namespace RcEngine {

class _ApiExport Component
{
public:
	Component(void);
	virtual ~Component(void);

	void SetOwner(GameObject* entity) { mOwner = entity; }

	virtual const std::string& GetComponentName() const = 0;
	virtual ComponentID GetComponentID() const { return GetIDFromName(GetComponentName()); }

	virtual void Init() {}
	virtual void Update(float deltaTime) {}
	virtual void Cleanup() {}

public:
	static ComponentID GetIDFromName(const String& componentName) 
	{
		return HashedString::CalculateHash(componentName);
	}




protected:
	GameObject* mOwner;
};

}

#endif // Component_h__