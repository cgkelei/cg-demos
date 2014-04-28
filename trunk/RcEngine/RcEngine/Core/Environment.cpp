#include <Core/Environment.h>

namespace RcEngine{

SINGLETON_DECL(Environment)

Environment::Environment()
	: mApplication(nullptr),
	  mRenderFactory(nullptr), 
	  mRenderDevice(nullptr)
{

}

Environment::~Environment()
{

}


} // Namespace RcEngine