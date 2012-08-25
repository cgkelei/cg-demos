#include <Scene/Node.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

Node::Node()
	: mParent(nullptr), mDirtyBits(NODE_DIRTY_ALL), 
	mPosition(0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f), mRotation(1.0f, 0.0f, 0.0f, 0.0f)
{

}

Node::Node( const String& name )
	: mName(name), mParent(nullptr), mDirtyBits(NODE_DIRTY_ALL), 
	  mPosition(0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f), mRotation(1.0f, 0.0f, 0.0f, 0.0f)
{

}

Node::~Node()
{

}


void Node::SetParent( Node* parent )
{
	mParent = parent;
}


void Node::SetPosition( const Vector3f& position )
{
	mPosition = position;
	TransformChanged();

}

void Node::SetRotation( const Quaternionf& rotation )
{
	mRotation = rotation;
	TransformChanged();
}

void Node::SetDirection( const Vector3f& direction )
{
	const Vector3f start(0.0f, 0.0f, 1.0f);		// forward direction
	const Vector3f end = Normalize(direction);

	Vector3f axis = Cross( start, end );
	float dot = Dot( start, end );

	SetRotation( Quaternionf( dot, axis.X(), axis.Y(), axis.Z() ) );
}

void Node::SetScale( const Vector3f& scale )
{
	mScale = scale;
	TransformChanged();
}

void Node::SetTransform( const Vector3f& position, const Quaternionf& rotation )
{
	mPosition = position;
	mRotation = rotation;
	TransformChanged();
}

void Node::SetTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale )
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	TransformChanged();
}

Matrix4f Node::GetTransform() const
{
	return CreateTransformMatrix(mScale, mRotation, mPosition);
}

void Node::SetWorldPosition( const Vector3f& position )
{
	if (!mParent)
	{
		SetPosition(position);
	}
	else
	{
		Matrix4f parentWorldTransformInv = mParent->GetWorldTransform().Inverse();
		SetPosition( Transform(position, parentWorldTransformInv) );
	}
}

Vector3f Node::GetWorldPosition() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return TranslationFromTransformMatrix(mWorldTransform);
}

void Node::SetWorldRotation( const Quaternionf& rotation )
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


Quaternionf Node::GetWorldRotation() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return QuaternionFromRotationMatrix( RotationFromTransformMatrix(mWorldTransform) );
}

void Node::SetWorldDirection( const Vector3f& direction )
{
	Vector3f localDirection;

	if (!mParent)
	{
		localDirection = direction;
	} 
	else
	{
		Matrix4f parentWorldTransformInv = mParent->GetWorldTransform().Inverse();
		localDirection = Transform(direction, parentWorldTransformInv);
	}

	SetDirection(localDirection);
}

Vector3f Node::GetWorldDirection() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	const Vector3f Froward(0.0f, 0.0f, 1.0f);
	return Transform(Froward, RotationFromTransformMatrix(mWorldTransform));
}

void Node::SetWorldScale( const Vector3f& scale )
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

Vector3f Node::GetWorldScale() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return ScaleFromTransformMatrix(mWorldTransform);
}


void Node::SetWorldTransform( const Vector3f& position, const Quaternionf& rotation )
{
	SetWorldPosition(position);
	SetWorldRotation(rotation);
}

void Node::SetWorldTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale )
{
	SetWorldPosition(position);
	SetWorldRotation(rotation);
	SetWorldScale(scale);
}

const Matrix4f& Node::GetWorldTransform() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return mWorldTransform;
}

uint32_t Node::GetNumChildren( bool recursive /*= false */ ) const
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


Node* Node::CreateChild( const String& name, const Vector3f& translate, const Quaternionf& rotate )
{
	Node* child = CreateChildImpl(name);
	child->SetPosition(translate);
	child->SetRotation(rotate);

	// Attach this child
	AttachChild(child);
	return child;
}


void Node::AttachChild( Node* child )
{
	// Check for illegal or redundant parent assignment
	if (!child || child == this || child->mParent == this)
		return;

	// Add first, then remove from old parent
	mChildren.push_back(child);

	if (child->mParent)
		child->mParent->DetachChild(child);

	// set new parent
	child->SetParent(this);

	child->TransformChanged();

	OnChildNodeAdded(child);
}

void Node::DetachChild( Node* child )
{
	// The child is not in our hierarchy.
	if (!child || child->mParent != this)	
		return;

	auto found =  std::find(mChildren.begin(), mChildren.end(), child);

	if (found != mChildren.end())
	{
		mChildren.erase(found);
		child->mParent = nullptr;
		child->TransformChanged();
	}

	OnChildNodeRemoved(child);
	
}

void Node::DetachAllChildren()
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->SetParent(nullptr);
		(*iter)->TransformChanged();
		OnChildNodeRemoved(*iter);
	}
	mChildren.clear();
}

Node* Node::GetChild( const String& name ) const
{
	auto found =  std::find_if( mChildren.begin(), mChildren.end(), [&name](Node* node){ 
						return node->GetName() == name;
					});

	if (found == mChildren.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Child node named " + name +
			" does not exist.", "Node::GetChild");
	}
	return *found;
}

void Node::OnChildNodeAdded( Node* node )
{

}

void Node::OnChildNodeRemoved( Node* node )
{

}

void Node::UpdateWorldTransform() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
	{
		if (mParent)
		{
			mParent->UpdateWorldTransform();
			mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition) * mParent->mWorldTransform;
		}
		else
		{
			mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition);
		}

		mDirtyBits &= ~NODE_DIRTY_WORLD;

		// force children to update their world transform.
		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			(*iter)->UpdateWorldTransform();
		}
	}
}

void Node::MarkBoundDirty()
{
	// Mark ourself and our parent nodes as dirty
	mDirtyBits |= NODE_DIRTY_BOUNDS;

	// Mark our parent bounds as dirty as well
	if (mParent)
		mParent->MarkBoundDirty();
}

void Node::TransformChanged()
{
	// Our local transform was changed, so mark our world matrices dirty.
	mDirtyBits |= NODE_DIRTY_WORLD;

	// World bound changes too, it may also cause parent bound dirty
	MarkBoundDirty();

	// Children node transform will change too, this will not mark parent's dirty again.
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		(*iter)->MarkChildrenDirty();
}

void Node::MarkChildrenDirty()
{
	mDirtyBits |= NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS;

	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		(*iter)->MarkChildrenDirty();
}

void Node::Update( )
{
}

}