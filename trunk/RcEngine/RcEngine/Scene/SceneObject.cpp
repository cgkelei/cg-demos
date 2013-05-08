#include <Scene/SceneObject.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Graphics/RenderQueue.h>

namespace RcEngine {

// undefined bounding sphere
static const BoundingBoxf UnDefineBoundingBox;


SceneObject::SceneObject( const String& name, SceneObejctType type, bool renderable /*= false*/ )
	: mName(name), mType(type), mRenderable(renderable), mParentNode(nullptr)
{

}

SceneObject::~SceneObject()
{

}

const BoundingBoxf& SceneObject::GetWorldBoundingBox() const
{
	return UnDefineBoundingBox;
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


void SceneObject::OnUpdateRenderQueue( RenderQueue* renderQueue, Camera* cam, RenderOrder order )
{

}



}