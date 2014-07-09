#ifndef GameObject_h__
#define GameObject_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport GameObject
{
public:
	explicit GameObject(GameObjectID id);
	~GameObject(void);

	inline GameObjectID GetID() const { return mID; }

	void SetActive(bool bActive);
	inline bool IsActive() const  { return mActive; }
	
	bool HasComponent(ComponentID id) const;

	template <typename T>
	T* GetComponent(ComponentID id) const 
	{
		auto it = mComponents.find(id);
		if (it != mComponents.end())
		{
			return static_cast<T*>(it->second);
		}

		return nullptr;
	}

	template <typename T>
	T* GetComponent(const String& name) const 
	{
		return GetComponent(Component::GetIDFromName(name))
	}

	void AddComponent(Component* component);

	// Removes the component from this object. Calls the OnDetachedFromObject method on the component.
	// The component is still valid afterwards. 
	Component* RemoveComponent(ComponentID componentID);
	void RemoveComponent(Component* component);

private:
	GameObjectID mID;

	bool mActive;

	unordered_map<ComponentID, Component*> mComponents;
};

class GameObjectFactory
{
public:
	GameObjectFactory()
		: mLastGameObjectID(0) {}

	inline GameObjectID GetNextGameObjectID() { ++mLastGameObjectID; return mLastGameObjectID; }

private:
	GameObjectID mLastGameObjectID;
};

}

#endif // GameObject_h__