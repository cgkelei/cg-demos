#include <Core/Environment.h>

namespace RcEngine{

SINGLETON_DECL(Environment)

Environment::Environment()
	: mApplication(nullptr),
	  mRHFactory(nullptr), 
	  mRHDevice(nullptr)
{

}

Environment::~Environment()
{

}


} // Namespace RcEngine