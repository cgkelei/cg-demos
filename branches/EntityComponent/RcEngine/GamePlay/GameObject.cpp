#include <GamePlay/GameObject.h>
#include <GamePlay/Component.h>

namespace RcEngine {

GameObject::GameObject( GameObjectID id )
	: mID(id),
	  mActive(false)
{

}


GameObject::~GameObject(void)
{
}

void GameObject::SetActive( bool bActive )
{
	mActive = bActive;
}

bool GameObject::HasComponent( ComponentID id ) const
{
	return mComponents.find(id) != mComponents.end();
}

void GameObject::AddComponent( Component* component )
{
	ComponentID key = component->GetComponentID();
	if (HasComponent(key))
	{
		// Exception
	}
	else
	{
		mComponents[key] = component;
		component->SetOwner(this);
	}
}

Component* GameObject::RemoveComponent( ComponentID componentID )
{
	Component* pComponent = nullptr;

	auto it = mComponents.find(componentID);
	if (it != mComponents.end())
	{
		pComponent = it->second;
		pComponent->SetOwner(nullptr);

		mComponents.erase(it);
	}

	return pComponent;
}

void GameObject::RemoveComponent( Component* pComponent )
{
	if (!pComponent) 
		return;
	
	auto it = mComponents.find(pComponent->GetComponentID());
	if (it != mComponents.end())
		mComponents.erase(it);

	pComponent->SetOwner(nullptr);
}

}