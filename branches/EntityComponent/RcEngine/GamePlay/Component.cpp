#include "Component.h"
#include "GamePlay/GameObject.h"

namespace RcEngine {

Component::Component(void)
{
}


Component::~Component(void)
{
}

void Component::DestroyComponent()
{
	if (mOwner)
	{
		mOwner->RemoveComponent(this);
	}
}

}