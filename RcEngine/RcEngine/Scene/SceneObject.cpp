#include <Scene/SceneObject.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>

namespace RcEngine {

// undefined bounding sphere
static const BoundingSpheref UnDefineBoundingSphere;


SceneObject::SceneObject( const String& name)
	: mParentNode(nullptr), mName(name)
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
	if (mParentNode)
	{
		return mParentNode->GetWorldTransform();
	}
	else
	{
		return Matrix4f::Identity();
	}
}

void SceneObject::OnAttach( Node* node )
{
	assert(!mParentNode || !node);
	mParentNode = node;
}

void SceneObject::OnDetach( Node* node )
{

}


void SceneObject::UpdateRenderQueue( RenderQueue& renderQueue, Camera* cam )
{

}

}