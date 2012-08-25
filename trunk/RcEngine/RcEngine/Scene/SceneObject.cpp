#include <Scene/SceneObject.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>

namespace RcEngine {

// undefined bounding sphere
static const BoundingSpheref UnDefineBoundingSphere;

SceneObject::SceneObject()
	: mParent(nullptr)
{

}

SceneObject::SceneObject( const String& name )
	: mParent(nullptr), mName(name)
{

}

SceneObject::~SceneObject()
{

}

const BoundingSpheref& SceneObject::GetWorldBoundingSphere() const
{
	return UnDefineBoundingSphere;
}


const BoundingSpheref& SceneObject::GetBoundingSphere() const
{
	return UnDefineBoundingSphere;
}


const Matrix4f& SceneObject::GetWorldTransform() const
{
	if (mParent)
	{
		return mParent->GetWorldTransform();
	}
	else
	{
		return Matrix4f::Identity();
	}
}

void SceneObject::OnAttach( SceneNode* node )
{
	assert(!mParent || !node);
	mParent = node;
}


void SceneObject::OnDetach( SceneNode* node )
{

}

}