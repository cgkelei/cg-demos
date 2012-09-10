#include <Graphics/Skeleton.h>

namespace RcEngine {

Joint::Joint( Skeleton* skeleton, const String& name )
	:Node(name)
{

}

Node* Joint::CreateChildImpl( const String& name )
{
	assert(mSkeleton);
	return nullptr;
}

void Joint::SetSkeleton( Skeleton* skeleton )
{

}

void Joint::OnUpdate( )
{

}


Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}


} // Namespace RcEngine
