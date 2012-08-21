#include <Scene/SceneNode.h>
#include <Scene/Scene.h>
#include <Math/MathUtil.h>

namespace RcEngine {

SceneNode::SceneNode()
{

}

SceneNode::~SceneNode()
{

}

void SceneNode::AttachChild( SceneNode* child )
{
	// Check for illegal or redundant parent assignment
	if (!child || child == this || child->mParent == this)
		return;

	// Add first, then remove from old parent
	mChildren.push_back(child);

	if (child->mParent)
		child->mParent->DetachChild(child);

	// Add to the scene if not added yet
	if (mScene && !child->mScene)
		mScene->AddSceneNode(child);

	// set new parent
	child->SetParent(this);

	//child->MarkDirty();
}

void SceneNode::DetachChild( SceneNode* child )
{
	// The child is not in our hierarchy.
	if (!child || child->mParent != this)	
		return;

	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		if ( *iter == child )
		{
			mChildren.erase(iter);
			break;
		}
	}

	child->mParent = nullptr;
	child->MarkDirty();

	Safe_Delete(child);
}

void SceneNode::RemoveAllChildren()
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		Safe_Delete(*iter);
		mChildren.erase(iter);	
	}
}

void SceneNode::SetParent( SceneNode* parent )
{

}

Matrix4f SceneNode::GetLocalTransform() const
{
	return CreateTransformMatrix(mScale, mRotation, mPosition);
}

Vector3f SceneNode::GetWorldPosition() const
{
	if (mDirty)
		UpdateWorldTransform();

	return TranslationFromTransformMatrix(mWorldTransform);
}

Quaternionf SceneNode::GetWorldRotation() const
{
	if (mDirty)
		UpdateWorldTransform();

	return QuaternionFromRotationMatrix( RotationFromTransformMatrix(mWorldTransform) );
}

Vector3f SceneNode::GetWorldDirection() const
{
	if (mDirty)
		UpdateWorldTransform();

	const Vector3f Froward(0.0f, 0.0f, 1.0f);
	return Transform(Froward, RotationFromTransformMatrix(mWorldTransform));
}

Vector3f SceneNode::GetWorldScale() const
{
	if (mDirty)
		UpdateWorldTransform();

	return ScaleFromTransformMatrix(mWorldTransform);
}

const Matrix4f& SceneNode::GetWorldTransform() const
{
	if (mDirty)
		UpdateWorldTransform();

	return mWorldTransform;
}

void SceneNode::MarkDirty()
{
	mDirty = true;

	// Mark child nodes
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->MarkDirty();
	}
}

void SceneNode::UpdateWorldTransform() const
{
	if (mParent)
	{
		if (mParent->IsDirty())
			mParent->UpdateWorldTransform();

		mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition) * mParent->mWorldTransform;
	}
	else
	{
		mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition);
	}

	mDirty = true;
}

void SceneNode::UpdateWorldBound() const
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* child = (*iter);
		if ( child->IsDirty() )
		{
			child->UpdateWorldBound();
		}

		mBoundingSphere.Merge(child->GetBoundingSphere());
	}
}

void SceneNode::SetPosition( const Vector3f& position )
{
	mPosition = position;
	if (mDirty)
	{
		MarkDirty();
	}
	
}

void SceneNode::SetRotation( const Quaternionf& rotation )
{
	mRotation = rotation;
	if (mDirty)
	{
		MarkDirty();
	}
}

void SceneNode::SetDirection( const Vector3f& direction )
{
	const Vector3f start(0.0f, 0.0f, 1.0f);		// forward direction
	const Vector3f end = Normalize(direction);
	
	Vector3f axis = Cross( start, end );
	float dot = Dot( start, end );

	SetRotation( Quaternionf( dot, axis.X(), axis.Y(), axis.Z() ) );
}

void SceneNode::SetScale( const Vector3f& scale )
{
	mScale = scale;
	if (mDirty)
	{
		MarkDirty();
	}
}

void SceneNode::SetTransform( const Vector3f& position, const Quaternionf& rotation )
{
	mPosition = position;
	mRotation = rotation;
}

void SceneNode::SetTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale )
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
}

void SceneNode::SetWorldPosition( const Vector3f& position )
{
	if (!mParent)
	{
		SetPosition(position);
	}
	else
	{
		Matrix4f parentWorldTransformInv = MatrixInverse( mParent->GetWorldTransform() );
		SetPosition( Transform(position, parentWorldTransformInv) );
	}
}

void SceneNode::SetWorldRotation( const Quaternionf& rotation )
{
	if (!mParent)
	{
		SetRotation(rotation);
	} 
	else
	{
		Quaternionf parentWorldRotInv = QuaternionInverse( mParent->GetWorldRotation() );
		SetRotation(rotation * parentWorldRotInv);
	}
}

void SceneNode::SetWorldDirection( const Vector3f& direction )
{
	Vector3f localDirection;

	if (!mParent)
	{
		localDirection = direction;
	} 
	else
	{
		Matrix4f parentWorldTransformInv = MatrixInverse( mParent->GetWorldTransform() );
		localDirection = Transform(direction, parentWorldTransformInv);
	}

	SetDirection(localDirection);
}

void SceneNode::SetWorldScale( const Vector3f& scale )
{
	if (!mParent)
	{
		SetScale(scale);
	} 
	else
	{
		Vector3f worldScale = mParent->GetWorldScale();
		SetScale(Vector3f( scale.X() / worldScale.X(), scale.Y() / worldScale.Y(), scale.Z() / worldScale.Z()));
	}
}

void SceneNode::SetWorldTransform( const Vector3f& position, const Quaternionf& rotation )
{
	SetWorldPosition(position);
	SetWorldRotation(rotation);
}

void SceneNode::SetWorldTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale )
{
	SetWorldPosition(position);
	SetWorldRotation(rotation);
	SetWorldScale(scale);
}

uint32_t SceneNode::GetNumChildren( bool recursive /*= false */ ) const
{
	if (!recursive)
		return mChildren.size();
	else
	{
		uint32_t count =  mChildren.size();
		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			count += (*iter)->GetNumChildren(true);
		}
		return count;
	}
	
}




}