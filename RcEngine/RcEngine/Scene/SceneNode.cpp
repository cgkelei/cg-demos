#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneObject.h>
#include <Graphics/Camera.h>
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

	// since we have attach a scene object, bound may invalid.
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);

	obj->OnAttach(this);
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

void SceneNode::UpdateWorldBounds() const
{
	if (mDirtyBits & NODE_DIRTY_BOUNDS)
	{
		mWorldBounds.SetNull();

		/**
		 * Merge all attached object's bound first, note that if object doesn't have bound,
		 * the Merge operation doesn't have effect.
		 */
		for (auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter)
		{
			mWorldBounds.Merge((*iter)->GetWorldBoundingSphere());
		}

		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			SceneNode* childNode = static_cast<SceneNode*>(*iter);
			mWorldBounds.Merge(childNode->GetWorldBoundingShpere());
		}

		 mDirtyBits &= ~NODE_DIRTY_BOUNDS;
	}
}

const BoundingSpheref& SceneNode::GetWorldBoundingShpere() const
{
	if (mDirtyBits & NODE_DIRTY_BOUNDS)
		UpdateWorldBounds();
	
	return mWorldBounds;
}

void SceneNode::OnPostUpdate()
{
	// update world bound
	UpdateWorldBounds();
}

void SceneNode::FindVisibleObjects( Camera* cam )
{
	if (!cam->Visible(mWorldBounds))
	{
		return;
	}

	for (auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter)
	{
		if ((*iter)->Renderable())
		{
			mScene->AddToRenderQueue(*iter);
		}	
	}

	// recursively call children
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* child = static_cast<SceneNode*>(*iter);
		child->FindVisibleObjects(cam);
	}
}

}