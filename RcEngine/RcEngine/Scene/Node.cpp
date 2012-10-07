#include <Scene/Node.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

Node::Node()
	: mParent(nullptr), mDirtyBits(NODE_DIRTY_ALL), 
	mPosition(Vector3f::Zero()), mRotation(Quaternionf::Identity()), mScale(1.0f, 1.0f, 1.0f)
{

}

Node::Node( const String& name, Node* parent )
	: mName(name), mParent(0), mDirtyBits(NODE_DIRTY_ALL), 
	  mPosition(Vector3f::Zero()), mRotation(Quaternionf::Identity()), mScale(1.0f, 1.0f, 1.0f)
{
	if (parent)
	{
		parent->AttachChild(this);
	}
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
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

void Node::SetRotation( const Quaternionf& rotation )
{
	mRotation = rotation;
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

void Node::SetScale( const Vector3f& scale )
{
	mScale = scale;
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

void Node::SetTransform( const Vector3f& position, const Quaternionf& rotation )
{
	mPosition = position;
	mRotation = rotation;
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

void Node::SetTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale )
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

Matrix4f Node::GetTransform() const
{
	// Order SRT
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
		// find the position in parent's local space
		Matrix4f parentWorldTransformInv = mParent->GetWorldTransform().Inverse();
		SetPosition( Transform(position, parentWorldTransformInv) );
	}
}

Vector3f Node::GetWorldPosition() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return TranslationFromMatrix(mWorldTransform);
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
		SetRotation( rotation * parentWorldRotInv );
	}
}


Quaternionf Node::GetWorldRotation() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return QuaternionFromRotationMatrix( RotationFromMatrix(mWorldTransform) );
}

Vector3f Node::GetWorldDirection() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	const Vector3f Froward(0.0f, 0.0f, 1.0f);
	return Transform(Froward, RotationFromMatrix(mWorldTransform));
}

Vector3f Node::GetWorldScale() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return ScaleFromMatrix(mWorldTransform);
}


void Node::SetWorldTransform( const Vector3f& position, const Quaternionf& rotation )
{
	if (!mParent)
	{
		mPosition = position;
		mRotation = rotation;
	}
	else
	{
		// find the position in parent's local space
		Matrix4f parentWorldTransformInv = mParent->GetWorldTransform().Inverse();
		SetPosition( Transform(position, parentWorldTransformInv) );

		Quaternionf parentWorldRotInv = QuaternionInverse( mParent->GetWorldRotation() );
		SetRotation( rotation * parentWorldRotInv );
	}

	PropagateDirtyDown(NODE_DIRTY_BOUNDS | NODE_DIRTY_WORLD);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

const Matrix4f& Node::GetWorldTransform() const
{
	if (mDirtyBits & NODE_DIRTY_WORLD)
		UpdateWorldTransform();

	return mWorldTransform;
}

void Node::Translate( const Vector3f& d, TransformSpace relativeTo /*= TS_Parent*/ )
{
	switch (relativeTo)
	{
	case TS_Local:
		{
			mPosition += Transform(d, mRotation);
		}
		break;
	case TS_World:
		{
			if (mParent)
			{
				Vector3f dInv = Transform(d, mParent->GetWorldRotation().Inverse());
				Vector3f scale = mParent->GetWorldScale();
				mPosition += Vector3f(dInv.X() / scale.X(), dInv.Y() / scale.Y(), dInv.Z() / scale.Z());
			}
			else
			{
				mPosition += d;
			}
		}
		break;
	case TS_Parent:
		{
			mPosition += d;
		}
		break;
	}

	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

void Node::Rotate( const Quaternionf& rot, TransformSpace relativeTo /*= TS_Parent */ )
{
	switch (relativeTo)
	{
	case TS_Local:
		{
			// ×ó³Ë£¬Local×îÏÈ³Ë
			mRotation = rot * mRotation;
		}
		break;
	case TS_World:
		{
			mRotation = GetWorldRotation() * rot * GetWorldRotation().Inverse() * mRotation;
		}
		break;
	case TS_Parent:
		{
			mRotation = mRotation * rot;
		}
		break;
	}

	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
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

	PropagateDirtyDown(NODE_DIRTY_BOUNDS | NODE_DIRTY_WORLD);
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);

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
		child->PropagateDirtyUp(NODE_DIRTY_BOUNDS);
	}

	OnChildNodeRemoved(child);
	
}

void Node::DetachAllChildren()
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->SetParent(nullptr);
		(*iter)->PropagateDirtyUp(NODE_DIRTY_BOUNDS);
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

void Node::Update( )
{
	if (! (mDirtyBits & NODE_DIRTY_WORLD) )
	{
		return;
	}

	OnPreUpdate();

	// Calculate absolute matrix

	if (mParent)
	{
		mParent->UpdateWorldTransform();
		mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition) * mParent->mWorldTransform;
	}
	else
	{
		mWorldTransform = CreateTransformMatrix(mScale, mRotation, mPosition);
	}

	OnPostUpdate();

	// clear dirty
	mDirtyBits &= ~NODE_DIRTY_WORLD;
	
	// Visit children
	for( size_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->Update();
	}	
}

void Node::PropagateDirtyDown( uint32_t dirtyFlag )
{
	mDirtyBits |= dirtyFlag;
	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->PropagateDirtyDown(dirtyFlag);
	}
}

void Node::PropagateDirtyUp( uint32_t dirtyFlag )
{
	mDirtyBits |= dirtyFlag;
	if (mParent)
	{
		mParent->PropagateDirtyUp(dirtyFlag);
	}
}

void Node::NeedUpdate()
{

}

}