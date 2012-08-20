#ifndef InputModule_h__
#define InputModule_h__

#include "Core/IModule.h"

namespace RcEngine{

		
class _ApiExport InputModule : public IModule
{
protected:
	InputModule();

public:
	virtual ~InputModule();
	virtual InputSystem* GetInputSystem() = 0;
};


} // Namespace RcEngine


#endif // InputModule_h__