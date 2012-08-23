#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneObject.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>


namespace RcEngine {

SceneNode::SceneNode( SceneManager* scene, const String& name )
	: Node(name)
{
	SetScene(scene);
}

SceneNode::~SceneNode()
{

}

void SceneNode::SetScene( SceneManager* scene )
{
	mScene = scene;
}


Node* SceneNode::CreateChildImpl( const String& name )
{
	assert(mScene);
	return mScene->CreateSceneNode(name);
}

SceneNode* SceneNode::CreateChildSceneNode( const String& name, const Vector3f& translate, const Quaternionf& rotate )
{
	return static_cast<SceneNode*>( CreateChild(name, translate, rotate) );
}

void SceneNode::AttachObject( SceneObject* obj )
{
	if (obj->IsAttached())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS,  "Object already attached to a SceneNode", "SceneNode::attachObject");
	}

	mAttachedObjects.push_back(obj);
	
	obj->OnAttach(this);

	// update bounding
}

void SceneNode::DetachOject( SceneObject* obj )
{

}

void SceneNode::DetachAllObject()
{

}

SceneObject* SceneNode::GetAttachedObject( const String& name )
{
	auto found = std::find_if(mAttachedObjects.begin(), mAttachedObjects.end(), [&name](SceneObject* obj){
						return obj->GetName() == name;
					});

	if (found == mAttachedObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Attached movable object named " + name +
			" does not exist.", "SceneNode::GetAttachedObject");
	}

	return *found;
}

void SceneNode::PropagateBoundToRoot()
{
	if (mParent)
	{
		UpdateWorldBounds();
		PropagateBoundToRoot();
	}
}

void SceneNode::UpdateWorldBounds() const
{
	mWorldBounds.SetNull();

	for (auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter)
	{
		mWorldBounds.Merge((*iter)->GetWorldBoundingSphere());
	}

	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* childNode = static_cast<SceneNode*>(*iter);
		mWorldBounds.Merge(childNode->GetWorldBoundingShpere());
	}
}

void SceneNode::OnUpdate( float tick )
{

}

const BoundingSpheref& SceneNode::GetWorldBoundingShpere() const
{
	if (mWorldBoundDirty)
	{
		// Do not use mWorldTransform directly, it's may out of date.
		const Matrix4f& worldMat = GetWorldTransform();



	}

	return mWorldBounds;
}

}